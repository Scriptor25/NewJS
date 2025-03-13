#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/value.hpp>

NJS::MemberExpression::MemberExpression(
    SourceLocation where,
    ExpressionPtr object,
    std::string member,
    const bool dereference)
    : Expression(std::move(where)),
      Object(std::move(object)),
      Member(std::move(member)),
      Dereference(dereference)
{
}

NJS::ValuePtr NJS::MemberExpression::PGenLLVM(Builder &builder, const TypePtr &)
{
    auto object = Object->GenLLVM(builder, {});
    if (Dereference)
    {
        if (!object->GetType()->IsPointer())
            Error(Where, "cannot dereference value of non-pointer type {}", object->GetType());
        const auto pointer_type = Type::As<PointerType>(object->GetType());
        object = LValue::Create(builder, pointer_type->GetElement(), object->Load(), pointer_type->IsConst());
    }
    return builder.CreateMember(object, Member).Value;
}

std::ostream &NJS::MemberExpression::Print(std::ostream &stream) const
{
    return Object->Print(stream) << '.' << Member;
}
