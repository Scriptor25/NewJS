#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::ValuePtr NJS::Builder::CreateCast(const ValuePtr& value, const TypePtr& type)
{
    return RValue::Create(*this, type, CreateCast(value->Load(), value->GetType(), type));
}

llvm::Value* NJS::Builder::CreateCast(llvm::Value* val, const TypePtr& src_type, const TypePtr& dst_type) const
{
    if (src_type == dst_type)
        return val;

    const auto ty = dst_type->GetLLVM(*this);

    if (src_type->IsInt())
    {
        if (dst_type->IsInt())
            return GetBuilder().CreateIntCast(val, ty, dst_type->IsSigned());
        if (dst_type->IsFP())
            return src_type->IsSigned() ? GetBuilder().CreateSIToFP(val, ty) : GetBuilder().CreateUIToFP(val, ty);
        if (dst_type->IsPointer())
            return GetBuilder().CreateIntToPtr(val, ty);
    }
    if (src_type->IsFP())
    {
        if (dst_type->IsInt())
            return dst_type->IsSigned() ? GetBuilder().CreateFPToSI(val, ty) : GetBuilder().CreateFPToUI(val, ty);
        if (dst_type->IsFP())
            return GetBuilder().CreateFPCast(val, ty);
    }
    if (src_type->IsPointer())
    {
        if (dst_type->IsInt())
            return GetBuilder().CreatePtrToInt(val, ty);
        if (dst_type->IsPointer())
            return GetBuilder().CreatePointerCast(val, ty);
    }

    Error("TODO");
}
