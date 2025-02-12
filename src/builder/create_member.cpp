#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::ValuePtr NJS::Builder::CreateMember(const SourceLocation &where, const ValuePtr &object, const std::string &name)
{
    const auto object_type = object->GetType();

    const auto [type_, index_] = object_type->GetMember(name);

    if (object->IsL())
    {
        const auto ty = object_type->GetLLVM(where, *this);
        const auto ptr = object->GetPtr(where);
        const auto gep = GetBuilder().CreateStructGEP(ty, ptr, index_);
        return LValue::Create(*this, type_, gep);
    }

    const auto value = GetBuilder().CreateExtractValue(object->Load(where), index_);
    return RValue::Create(*this, type_, value);
}
