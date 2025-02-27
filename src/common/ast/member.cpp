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

NJS::ValuePtr NJS::MemberExpression::GenLLVM(Builder &builder, ErrorInfo &error, const TypePtr &) const
{
    const auto value = Object->GenLLVM(builder, error, {});
    return builder.CreateMember(Where, value, Member);
}

std::ostream &NJS::MemberExpression::Print(std::ostream &stream)
{
    return Object->Print(stream) << '.' << Member;
}
