#include <NJS/AST.hpp>

NJS::MemberExpr::MemberExpr(TypePtr type, ExprPtr object, std::string member)
    : Expr(std::move(type)), Object(std::move(object)), Member(std::move(member))
{
}

NJS::ValuePtr NJS::MemberExpr::GenLLVM(Builder& builder)
{
}

std::ostream& NJS::MemberExpr::Print(std::ostream& os)
{
    return os << Object << '.' << Member;
}
