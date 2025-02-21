#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
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
            args.push_back(builder.GetBuilder().getInt32(ID_INTEGER));
            args.push_back(builder.GetBuilder().getInt32(8));
            args.push_back(builder.GetBuilder().getInt32(1));
            args.push_back(string_value);

            continue;
        }
        if (DynamicExpressions.contains(i))
        {
            auto &dynamic = DynamicExpressions.at(i);
            const auto value = dynamic->GenLLVM(builder, {});
            value->GetType()->TypeInfo(dynamic->Where, builder, args);
            if (value->GetType()->IsPrimitive())
                args.push_back(value->Load(dynamic->Where));
            else if (value->IsLValue())
                args.push_back(value->GetPtr(dynamic->Where));
            else
            {
                const auto const_ref = builder.CreateAlloca(dynamic->Where, value->GetType(), true);
                const_ref->StoreForce(dynamic->Where, value);
                args.push_back(const_ref->GetPtr(dynamic->Where));
            }

            continue;
        }

        Error(Where, "non-existent formatter operand at index {}", i);
    }

    args.push_back(builder.GetBuilder().getInt32(ID_VOID));

    llvm::FunctionCallee format_callee;
    builder.GetFormat(format_callee);
    builder.GetBuilder().CreateCall(format_callee, args);

    return RValue::Create(builder, builder.GetTypeContext().GetStringType(), buffer_pointer);
}

std::ostream &NJS::FormatExpression::Print(std::ostream &stream)
{
    stream << "$\"";
    for (unsigned i = 0; i < Count; ++i)
    {
        if (StaticExpressions.contains(i))
            stream << StaticExpressions[i];
        else if (DynamicExpressions.contains(i))
            DynamicExpressions[i]->Print(stream << '{') << '}';
    }
    return stream << '"';
}
