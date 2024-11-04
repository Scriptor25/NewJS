#include <NJS/AST.hpp>

NJS::MemberExpr::MemberExpr(ExprPtr object, std::string member)
    : Object(std::move(object)), Member(std::move(member))
{
}

NJS::ValuePtr NJS::MemberExpr::GenLLVM(Builder& builder)
{
}

std::ostream& NJS::MemberExpr::Print(std::ostream& os)
{
    return Object->Print(os) << '.' << Member;
}
