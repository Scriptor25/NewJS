#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Std.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::FormatExpression::FormatExpression(
    SourceLocation where,
    const unsigned count,
    std::map<unsigned, std::string> static_expressions,
    std::map<unsigned, ExpressionPtr> dynamic_expressions)
    : Expression(std::move(where)),
      Count(count),
      StaticExpressions(std::move(static_expressions)),
      DynamicExpressions(std::move(dynamic_expressions))
{
}

NJS::ValuePtr NJS::FormatExpression::GenLLVM(Builder &builder, const TypePtr &) const
{
    constexpr auto BUFFER_SIZE = 1024;

    const auto buffer_pointer = builder.CreateAlloca(builder.GetBuilder().getInt8Ty(), BUFFER_SIZE);

    std::vector<llvm::Value *> args;

    args.push_back(buffer_pointer);
    args.push_back(builder.GetBuilder().getInt64(BUFFER_SIZE));

    for (unsigned i = 0; i < Count; ++i)
    {
        if (StaticExpressions.contains(i))
        {
            const auto value = StaticExpressions.at(i);
            const auto string_value = StringExpression::GetString(builder, value);

            args.push_back(builder.GetBuilder().getInt32(ID_POINTER));
            args.push_back(builder.GetBuilder().getInt32(ID_INT));
            args.push_back(builder.GetBuilder().getInt32(8));
            args.push_back(builder.GetBuilder().getInt32(1));
            args.push_back(string_value);
        }
        else if (DynamicExpressions.contains(i))
        {
            const auto value = DynamicExpressions.at(i)->GenLLVM(builder, {});
            value->GetType()->TypeInfo(Where, builder, args);
            if (value->GetType()->IsPrimitive())
                args.push_back(value->Load(Where));
            else if (value->IsL())
                args.push_back(value->GetPtr(Where));
            else
            {
                const auto value_pointer = builder.CreateAlloca(Where, value->GetType());
                value_pointer->Store(Where, value);
                args.push_back(value_pointer->GetPtr(Where));
            }
        }
    }

    args.push_back(builder.GetBuilder().getInt32(ID_VOID));

    llvm::FunctionCallee format_callee;
    builder.GetFormat(format_callee);
    builder.GetBuilder().CreateCall(format_callee, args);

    return RValue::Create(builder, builder.GetTypeContext().GetStringType(), buffer_pointer);
}

std::ostream &NJS::FormatExpression::Print(std::ostream &os)
{
    os << "$\"";
    for (unsigned i = 0; i < Count; ++i)
    {
        if (StaticExpressions.contains(i))
            os << StaticExpressions[i];
        else if (DynamicExpressions.contains(i))
            DynamicExpressions[i]->Print(os << '{') << '}';
    }
    return os << '"';
}
