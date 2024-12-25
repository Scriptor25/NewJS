#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>

NJS::MemberExpr::MemberExpr(SourceLocation where, ExprPtr object, std::string member)
    : Expr(std::move(where)), Object(std::move(object)), Member(std::move(member))
{
}

NJS::ValuePtr NJS::MemberExpr::GenLLVM(Builder& builder, const TypePtr&)
{
    const auto obj = Object->GenLLVM(builder, {});
    return builder.CreateMember(Where, obj, Member);
}

std::ostream& NJS::MemberExpr::Print(std::ostream& os)
{
    return Object->Print(os) << '.' << Member;
}
