#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/type.hpp>

NJS::MemberExpression::MemberExpression(SourceLocation where, ExpressionPtr object, std::string member)
    : Expression(std::move(where)),
      Object(std::move(object)),
      Member(std::move(member))
{
}

NJS::ValuePtr NJS::MemberExpression::PGenLLVM(Builder &builder, const TypePtr &) const
{
    return builder.CreateMember(Object->GenLLVM(builder, {}), Member);
}

std::ostream &NJS::MemberExpression::Print(std::ostream &stream) const
{
    return Object->Print(stream) << '.' << Member;
}
