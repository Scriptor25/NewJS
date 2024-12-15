#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::ValuePtr NJS::Builder::CreateMember(const ValuePtr& obj, const std::string& name)
{
    const auto obj_type = obj->GetType();

    if (obj_type->IsVector() && name == "length")
    {
        const auto ptr_to_size = CreatePtrToVectorSize(obj->Load());
        const auto size = GetBuilder().CreateLoad(GetBuilder().getInt64Ty(), ptr_to_size);
        const auto length = GetBuilder().CreateUIToFP(size, GetBuilder().getDoubleTy());
        return RValue::Create(*this, GetCtx().GetNumberType(), length);
    }

    const auto [type_, index_] = obj_type->Member(name);

    if (index_ == ~0)
        Error("no member '{}' in {}", name, obj_type);

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
