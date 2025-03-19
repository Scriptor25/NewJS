#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/value.hpp>

NJS::MemberValue NJS::Builder::CreateMember(const ValuePtr &value, const std::string &name)
{
    if (!value->GetType()->IsStruct())
        Error("cannot access member of non-struct value of type {}", value->GetType());
    const auto struct_type = Type::As<StructType>(value->GetType());

    const auto [
        index_,
        name_,
        info_,
        default_
    ] = struct_type->GetMember(name);

    return CreateMember(value, index_);
}

NJS::MemberValue NJS::Builder::CreateMember(const ValuePtr &value, const unsigned index)
{
    if (!value->GetType()->IsStruct())
        Error("cannot access member of non-struct value of type {}", value->GetType());
    const auto struct_type = Type::As<StructType>(value->GetType());

    const auto [
        index_,
        name_,
        info_,
        default_
    ] = struct_type->GetMember(index);
    const auto [
        type_,
        is_const_,
        is_reference_
    ] = info_;

    if (value->IsLValue())
    {
        const auto type = struct_type->GetLLVM(*this);
        const auto pointer = value->GetPointer();

        const auto gep = GetBuilder().CreateStructGEP(type, pointer, index_);
        if (is_reference_)
            return {
                LValue::Create(*this, type_, GetBuilder().CreateLoad(GetBuilder().getPtrTy(), gep), is_const_),
                true
            };
        if (is_const_)
            return {CreateCast(default_->GenLLVM(*this, type_), type_), false};
        return {LValue::Create(*this, type_, gep, value->IsConst() || is_const_), false};
    }

    const auto member_value = GetBuilder().CreateExtractValue(value->Load(), index_);
    if (is_reference_)
        return {LValue::Create(*this, type_, member_value, is_const_), true};
    return {RValue::Create(*this, type_, member_value), false};
}
