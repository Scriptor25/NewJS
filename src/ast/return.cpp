#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::ReturnStmt::ReturnStmt(SourceLocation where, ExprPtr value)
    : Stmt(std::move(where)), Value(std::move(value))
{
}

void NJS::ReturnStmt::GenVoidLLVM(Builder& builder)
{
    auto type = builder.ResultType();
    const auto ref = type->IsRef();
    if (ref) type = type->GetElement();

    auto value = Value->GenLLVM(builder, type);
    value = builder.CreateCast(Where, value, type);

    if (ref)
        builder.GetBuilder().CreateRet(value->GetPtr(Where));
    else builder.GetBuilder().CreateRet(value->Load(Where));
}

std::ostream& NJS::ReturnStmt::Print(std::ostream& os)
{
    if (Value)
        return Value->Print(os << "return ");
    return os << "return";
}
