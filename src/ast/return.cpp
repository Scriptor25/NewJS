#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::ReturnStmt::ReturnStmt(SourceLocation where, ExprPtr value)
    : Stmt(std::move(where)), Value(std::move(value))
{
}

NJS::ValuePtr NJS::ReturnStmt::GenLLVM(Builder& builder)
{
    const auto type = builder.ResultType();
    const auto value = Value->GenLLVM(builder);

    if ((type->IsRef() && type->GetElement() != value->GetType()) || (!type->IsRef() && type != value->GetType()))
        Error(Where, "invalid return value: type mismatch, {} != {}", value->GetType(), type);

    if (type->IsRef())
        builder.GetBuilder().CreateRet(value->GetPtr());
    else builder.GetBuilder().CreateRet(value->Load());

    return {};
}

std::ostream& NJS::ReturnStmt::Print(std::ostream& os)
{
    if (Value)
        return Value->Print(os << "return ");
    return os << "return";
}
