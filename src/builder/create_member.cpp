#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::ValuePtr NJS::Builder::CreateMember(const ValuePtr& obj, const std::string& name)
{
    const auto obj_type = obj->GetType();
    const auto [type_, index_] = obj_type->Member(name);

    if (obj->IsL())
    {
        const auto ty = obj_type->GenLLVM(*this);
        const auto ptr = obj->GetPtr();
        const auto gep = GetBuilder().CreateStructGEP(ty, ptr, index_);
        return LValue::Create(*this, type_, gep);
    }

    const auto value = GetBuilder().CreateExtractValue(obj->Load(), index_);
    return RValue::Create(*this, type_, value);
}
