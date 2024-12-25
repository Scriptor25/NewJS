#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::ValuePtr NJS::Builder::CreateCast(const SourceLocation& where, const ValuePtr& value, const TypePtr& type)
{
    if (value->GetType() == type)
        return value;

    const auto result = CreateCast(
        where,
        {value->Load(), value->IsL() ? value->GetPtr(where) : nullptr},
        value->GetType(),
        type);
    return RValue::Create(*this, type, result);
}

llvm::Value* NJS::Builder::CreateCast(
    const SourceLocation& where,
    ValRef ref,
    const TypePtr& src_type,
    const TypePtr& dst_type) const
{
    const auto [val, ptr] = ref;

    if (src_type == dst_type)
        return val;

    const auto ty = dst_type->GetLLVM(*this);

    if (src_type->IsInt())
    {
        if (dst_type->IsInt())
            return GetBuilder().CreateIntCast(val, ty, dst_type->IsSigned());
        if (dst_type->IsFP())
            return src_type->IsSigned() ? GetBuilder().CreateSIToFP(val, ty) : GetBuilder().CreateUIToFP(val, ty);
        if (dst_type->IsPtr())
            return GetBuilder().CreateIntToPtr(val, ty);
    }
    if (src_type->IsFP())
    {
        if (dst_type->IsInt())
            return dst_type->IsSigned() ? GetBuilder().CreateFPToSI(val, ty) : GetBuilder().CreateFPToUI(val, ty);
        if (dst_type->IsFP())
            return GetBuilder().CreateFPCast(val, ty);
    }
    if (src_type->IsPtr())
    {
        if (dst_type->IsInt())
            return GetBuilder().CreatePtrToInt(val, ty);
        if (dst_type->IsPtr())
            return GetBuilder().CreatePointerCast(val, ty);
    }
    if (ptr && src_type->IsArray())
    {
        if (dst_type->IsPtr() && src_type->GetElement() == dst_type->GetElement())
            return GetBuilder().CreateConstGEP2_64(src_type->GetLLVM(*this), ptr, 0, 0);
    }
    if (src_type->IsFunction())
    {
        if (dst_type->IsFunction())
            return GetBuilder().CreatePointerCast(val, ty);
    }

    Error(where, "no cast from value of type {} to {}", src_type, dst_type);
}
