#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::ReturnStatement::ReturnStatement(SourceLocation where, ExpressionPtr value)
    : Statement(std::move(where)),
      Value(std::move(value))
{
}

void NJS::ReturnStatement::GenVoidLLVM(Builder &builder) const
{
    auto &[type_, is_const_, is_reference_] = builder.CurrentFunctionResult();

    if (!Value)
    {
        if (!type_->IsVoid())
            Error(Where, "cannot return void from a function with non-void return type '{}'", type_);
        builder.GetBuilder().CreateRetVoid();
        return;
    }

    auto value = Value->GenLLVM(builder, type_);
    value = builder.CreateCast(Value->Where, value, type_);

    if (is_reference_)
    {
        if (value->GetType() != type_)
            Error(
                Where,
                "type mismatch: cannot return reference with type {} from value of type {}",
                type_,
                value->GetType());
        if (value->IsConst() && !is_const_)
            Error(Where, "cannot reference constant value as mutable");
        builder.GetBuilder().CreateRet(value->GetPtr(Value->Where));
        return;
    }

    builder.GetBuilder().CreateRet(value->Load(Value->Where));
}

std::ostream &NJS::ReturnStatement::Print(std::ostream &stream)
{
    if (Value)
        return Value->Print(stream << "return ");
    return stream << "return void";
}
