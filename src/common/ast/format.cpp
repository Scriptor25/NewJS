#include <utility>
#include <llvm/IR/InlineAsm.h>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/std.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

NJS::ConstantFormatNode::ConstantFormatNode(std::string value)
    : Value(std::move(value))
{
}

std::ostream &NJS::ConstantFormatNode::Print(std::ostream &stream) const
{
    return stream << Value;
}

void NJS::ConstantFormatNode::Generate(Builder &builder, std::vector<llvm::Value *> &arguments) const
{
    const auto value = builder.GetString(Value);

    arguments.emplace_back(builder.GetBuilder().getInt32(ID_POINTER));
    arguments.emplace_back(builder.GetBuilder().getInt32(ID_INTEGER));
    arguments.emplace_back(builder.GetBuilder().getInt32(8));
    arguments.emplace_back(builder.GetBuilder().getInt32(1));
    arguments.emplace_back(value);
}

NJS::ExpressionFormatNode::ExpressionFormatNode(ExpressionPtr value)
    : Value(std::move(value))
{
}

std::ostream &NJS::ExpressionFormatNode::Print(std::ostream &stream) const
{
    return Value->Print(stream << "${") << '}';
}

void NJS::ExpressionFormatNode::Generate(Builder &builder, std::vector<llvm::Value *> &arguments) const
{
    const auto value = Value->GenIntermediate(builder, nullptr);
    const auto size = arguments.size();

    if (value->GetType()->TypeInfo(builder, arguments))
    {
        arguments.resize(size);
        return;
    }

    if (value->GetType()->IsPrimitive())
    {
        arguments.emplace_back(value->Load());
        return;
    }

    if (value->IsLValue())
    {
        arguments.emplace_back(value->GetPointer());
        return;
    }

    const auto alloc = builder.CreateAlloca(value->GetType(), true);
    alloc->StoreNoError(value);

    arguments.emplace_back(alloc->GetPointer());
}

NJS::FormatExpression::FormatExpression(
    SourceLocation where,
    std::vector<FormatNodePtr> nodes)
    : Expression(std::move(where)),
      Nodes(std::move(nodes))
{
}

std::ostream &NJS::FormatExpression::Print(std::ostream &stream) const
{
    stream << '`';
    for (const auto &node: Nodes)
        node->Print(stream);
    return stream << '`';
}

NJS::ValuePtr NJS::FormatExpression::_GenIntermediate(Builder &builder, const TypePtr &)
{
    std::vector<llvm::Value *> arguments;
    arguments.emplace_back(builder.GetBuilder().getInt32(Nodes.size()));

    for (const auto &node: Nodes)
        node->Generate(builder, arguments);

    arguments.emplace_back(builder.GetBuilder().getInt32(ID_VOID));

    llvm::FunctionCallee format_callee;
    builder.GetFormat(format_callee);
    const auto buffer = builder.GetBuilder().CreateCall(format_callee, arguments);

    return RValue::Create(builder, builder.GetTypeContext().GetStringType(), buffer);
}
