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

void NJS::ReturnStatement::GenLLVM(Builder &builder) const
{
    auto &[
        type_,
        is_const_,
        is_reference_
    ] = builder.CurrentFunctionResult();

    if (!Value)
    {
        if (!type_->IsVoid())
            return;

        builder.GetBuilder().CreateRetVoid();
        return;
    }

    auto value = Value->GenLLVM(builder, type_);
    value = builder.CreateCast(value, type_);

    if (is_reference_)
    {
        if (value->GetType() != type_)
            return;

        if (value->IsConst() && !is_const_)
            return;

        const auto pointer = value->GetPointer();

        builder.GetBuilder().CreateRet(pointer);
        return;
    }

    builder.GetBuilder().CreateRet(value->Load());
}

std::ostream &NJS::ReturnStatement::Print(std::ostream &stream)
{
    if (Value)
        return Value->Print(stream << "return ");
    return stream << "return void";
}
