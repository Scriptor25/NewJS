#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/std.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

NJS::FormatExpression::FormatExpression(
    SourceLocation where,
    const unsigned operand_count,
    std::map<unsigned, std::string> static_expressions,
    std::map<unsigned, ExpressionPtr> dynamic_operands)
    : Expression(std::move(where)),
      OperandCount(operand_count),
      StaticOperands(std::move(static_expressions)),
      DynamicOperands(std::move(dynamic_operands))
{
}

NJS::ValuePtr NJS::FormatExpression::PGenLLVM(Builder &builder, const TypePtr &) const
{
    std::vector<llvm::Value *> arguments;
    arguments.emplace_back(builder.GetBuilder().getInt32(OperandCount));
    for (unsigned i = 0; i < OperandCount; ++i)
    {
        if (StaticOperands.contains(i))
        {
            const auto value = StaticOperands.at(i);
            const auto string_value = builder.GetString(value);

            arguments.emplace_back(builder.GetBuilder().getInt32(ID_POINTER));
            arguments.emplace_back(builder.GetBuilder().getInt32(ID_INTEGER));
            arguments.emplace_back(builder.GetBuilder().getInt32(8));
            arguments.emplace_back(builder.GetBuilder().getInt32(1));
            arguments.emplace_back(string_value);

            continue;
        }

        if (DynamicOperands.contains(i))
        {
            auto &dynamic = DynamicOperands.at(i);

            const auto value = dynamic->GenLLVM(builder, {});
            const auto size = arguments.size();

            if (value->GetType()->TypeInfo(builder, arguments))
            {
                arguments.resize(size);
                continue;
            }

            if (value->GetType()->IsPrimitive())
            {
                arguments.emplace_back(value->Load());
                continue;
            }

            if (value->IsLValue())
            {
                arguments.emplace_back(value->GetPointer());
                continue;
            }

            const auto alloc = builder.CreateAlloca(value->GetType(), true);
            alloc->StoreNoError(value);
            arguments.emplace_back(alloc->GetPointer());
            continue;
        }

        Error(Where, "invalid format operand index {}", i);
    }

    arguments.emplace_back(builder.GetBuilder().getInt32(ID_VOID));

    llvm::FunctionCallee format_callee;
    builder.GetFormat(format_callee);
    const auto buffer = builder.GetBuilder().CreateCall(format_callee, arguments);

    return RValue::Create(builder, builder.GetTypeContext().GetStringType(), buffer);
}

std::ostream &NJS::FormatExpression::Print(std::ostream &stream) const
{
    stream << "f\"";
    for (unsigned i = 0; i < OperandCount; ++i)
    {
        if (StaticOperands.contains(i))
            stream << StaticOperands.at(i);
        else if (DynamicOperands.contains(i))
            DynamicOperands.at(i)->Print(stream << '{') << '}';
    }
    return stream << '"';
}
