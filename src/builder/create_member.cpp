#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::ValuePtr NJS::Builder::CreateMember(const SourceLocation& where, const ValuePtr& obj, const std::string& name)
{
    const auto obj_type = obj->GetType();

    const auto [type_, index_] = obj_type->GetMember(name);

    if (index_ == ~0)
        Error(where, "no member '{}' in {}", name, obj_type);

    if (obj->IsL())
    {
        const auto ty = obj_type->GetLLVM(where, *this);
        const auto ptr = obj->GetPtr(where);
        const auto gep = GetBuilder().CreateStructGEP(ty, ptr, index_);
        return LValue::Create(*this, type_, gep);
    }

    const auto value = GetBuilder().CreateExtractValue(obj->Load(where), index_);
    return RValue::Create(*this, type_, value);
}
