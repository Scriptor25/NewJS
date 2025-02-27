#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/value.hpp>

NJS::ValuePtr NJS::Builder::CreateMember(const SourceLocation &where, const ValuePtr &value, const std::string &name)
{
    const auto value_type = value->GetType();

    const auto [index_, name_, type_] = value_type->GetMember(where, name);

    if (value->IsLValue())
    {
        const auto ty = value_type->GetLLVM(where, *this);
        const auto ptr = value->GetPtr(where);
        const auto gep = GetBuilder().CreateStructGEP(ty, ptr, index_);
        return LValue::Create(*this, type_, gep, value->IsConst());
    }

    const auto member_value = GetBuilder().CreateExtractValue(value->Load(where), index_);
    return RValue::Create(*this, type_, member_value);
}
