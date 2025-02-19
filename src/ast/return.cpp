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
    auto type = builder.CurrentFunctionResultType();

    if (!Value)
    {
        if (!type->IsVoid())
            Error(Where, "cannot return void from a function with non-void return type '{}'", type);
        builder.GetBuilder().CreateRetVoid();
        return;
    }

    const auto ref = type->IsReference();
    if (ref)
        type = type->GetElement();

    auto value = Value->GenLLVM(builder, type);
    value = builder.CreateCast(Where, value, type);

    if (ref)
        builder.GetBuilder().CreateRet(value->GetPtr(Where));
    else
        builder.GetBuilder().CreateRet(value->Load(Where));
}

std::ostream &NJS::ReturnStatement::Print(std::ostream &stream)
{
    if (Value)
        return Value->Print(stream << "return ");
    return stream << "return";
}
