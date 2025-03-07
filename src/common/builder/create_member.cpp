#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/value.hpp>

NJS::ValuePtr NJS::Builder::CreateMember(const ValuePtr &value, const std::string &name)
{
    if (!value->GetType()->IsStruct())
        Error("cannot access member of non-struct value of type {}", value->GetType());
    const auto struct_type = Type::As<StructType>(value->GetType());

    const auto [
        index_,
        name_,
        info_
    ] = struct_type->GetMember(name);
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
        auto member_value = LValue::Create(*this, type_, gep, value->IsConst());
        if (is_reference_)
            return LValue::Create(*this, type_, member_value->Load(), is_const_);
        return member_value;
    }

    const auto member_value = GetBuilder().CreateExtractValue(value->Load(), index_);
    if (is_reference_)
        return LValue::Create(*this, type_, member_value, is_const_);
    return RValue::Create(*this, type_, member_value);
}
