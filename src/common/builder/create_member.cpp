#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/value.hpp>

NJS::ValuePtr NJS::Builder::CreateMember(const ValuePtr &value, const std::string &name)
{
    const auto value_type = value->GetType();

    const auto [
        index_,
        name_,
        type_
    ] = Type::As<StructType>(value_type)->GetMember(name);

    if (value->IsLValue())
    {
        const auto type = value_type->GetLLVM(*this);
        const auto pointer = value->GetPointer();

        const auto gep = GetBuilder().CreateStructGEP(type, pointer, index_);
        return LValue::Create(*this, type_, gep, value->IsConst());
    }

    const auto member_value = GetBuilder().CreateExtractValue(value->Load(), index_);
    return RValue::Create(*this, type_, member_value);
}
