#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Value.hpp>

NJS::ReturnStmt::ReturnStmt(SourceLocation where, ExprPtr value)
    : Stmt(std::move(where)), Value(std::move(value))
{
}

NJS::ValuePtr NJS::ReturnStmt::GenLLVM(Builder& builder)
{
    const auto value = Value->GenLLVM(builder);
    builder.GetBuilder().CreateRet(value->Load());
    return {};
}

std::ostream& NJS::ReturnStmt::Print(std::ostream& os)
{
    if (Value)
        return Value->Print(os << "return ");
    return os << "return";
}
