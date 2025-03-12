#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/error.hpp>
#include <newjs/type.hpp>
#include <newjs/value.hpp>

NJS::ReturnStatement::ReturnStatement(SourceLocation where, ExpressionPtr value)
    : Statement(std::move(where)),
      Value(std::move(value))
{
}

void NJS::ReturnStatement::PGenLLVM(Builder &builder) const
{
    auto &info = builder.CurrentFunctionResult();

    if (!Value)
    {
        if (!info.Type->IsVoid())
            Error(Where, "cannot return void for non-void return type {}", info.Type);

        builder.GetBuilder().CreateRetVoid();
        return;
    }

    const auto value = Value->GenLLVM(builder, info.Type);
    const auto result = info.SolveFor(builder, value);

    builder.GetBuilder().CreateRet(result);
}

std::ostream &NJS::ReturnStatement::Print(std::ostream &stream) const
{
    if (Value)
        return Value->Print(stream << "return ");
    return stream << "return void";
}
