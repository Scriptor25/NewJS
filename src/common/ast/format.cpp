#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/error.hpp>
#include <newjs/std.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

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

    std::vector<llvm::Value *> arguments;

    arguments.emplace_back(buffer_pointer);
    arguments.emplace_back(builder.GetBuilder().getInt64(BUFFER_SIZE));

    for (unsigned i = 0; i < Count; ++i)
    {
        if (StaticExpressions.contains(i))
        {
            const auto value = StaticExpressions.at(i);
            const auto string_value = StringExpression::GetString(builder, value);

            arguments.emplace_back(builder.GetBuilder().getInt32(ID_POINTER));
            arguments.emplace_back(builder.GetBuilder().getInt32(ID_INTEGER));
            arguments.emplace_back(builder.GetBuilder().getInt32(8));
            arguments.emplace_back(builder.GetBuilder().getInt32(1));
            arguments.emplace_back(string_value);

            continue;
        }
        if (DynamicExpressions.contains(i))
        {
            auto &dynamic = DynamicExpressions.at(i);
            const auto value = dynamic->GenLLVM(builder, {});
            const auto size = arguments.size();

            if (value->GetType()->TypeInfo(dynamic->Where, builder, arguments))
            {
                arguments.resize(size);
                continue;
            }

            if (value->GetType()->IsPrimitive())
            {
                arguments.emplace_back(value->Load(dynamic->Where));
                continue;
            }

            if (value->IsLValue())
            {
                arguments.emplace_back(value->GetPtr(dynamic->Where));
                continue;
            }

            const auto const_ref = builder.CreateAlloca(dynamic->Where, value->GetType(), true);
            const_ref->StoreForce(dynamic->Where, value);
            arguments.emplace_back(const_ref->GetPtr(dynamic->Where));
            continue;
        }

        Error(Where, "non-existent formatter operand at index {}", i);
    }

    arguments.emplace_back(builder.GetBuilder().getInt32(ID_VOID));

    llvm::FunctionCallee format_callee;
    builder.GetFormat(format_callee);
    builder.GetBuilder().CreateCall(format_callee, arguments);

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
