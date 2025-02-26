#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/value.hpp>

NJS::ValuePtr NJS::Builder::CreateMember(const SourceLocation &where, const ValuePtr &object, const std::string &name)
{
    const auto object_type = object->GetType();

    const auto [index_, name_, type_] = object_type->GetMember(where, name);

    if (object->IsLValue())
    {
        const auto ty = object_type->GetLLVM(where, *this);
        const auto ptr = object->GetPtr(where);
        const auto gep = GetBuilder().CreateStructGEP(ty, ptr, index_);
        return LValue::Create(*this, type_, gep, object->IsConst());
    }

    const auto value = GetBuilder().CreateExtractValue(object->Load(where), index_);
    return RValue::Create(*this, type_, value);
}
