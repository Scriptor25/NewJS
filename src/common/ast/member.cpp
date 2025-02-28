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

NJS::ValuePtr NJS::MemberExpression::GenLLVM(Builder &builder, const TypePtr &) const
{
    const auto object = Object->GenLLVM(builder, {});
    if (!object)
        return nullptr;
    return builder.CreateMember(object, Member);
}

std::ostream &NJS::MemberExpression::Print(std::ostream &stream)
{
    return Object->Print(stream) << '.' << Member;
}
