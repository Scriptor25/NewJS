#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>

NJS::MemberExpression::MemberExpression(SourceLocation where, ExpressionPtr object, std::string member)
    : Expression(std::move(where)),
      Object(std::move(object)),
      Member(std::move(member))
{
}

NJS::ValuePtr NJS::MemberExpression::GenLLVM(Builder &builder, const TypePtr &) const
{
    const auto object_value = Object->GenLLVM(builder, {});
    return builder.CreateMember(Where, object_value, Member);
}

std::ostream &NJS::MemberExpression::Print(std::ostream &stream)
{
    return Object->Print(stream) << '.' << Member;
}
