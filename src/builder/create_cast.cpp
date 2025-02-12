#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::ValuePtr NJS::Builder::CreateCast(const SourceLocation &where, const ValuePtr &value, const TypePtr &type)
{
    if (value->GetType() == type)
        return value;

    const auto result = CreateCast(
        where,
        {value->Load(where), value->IsL() ? value->GetPtr(where) : nullptr},
        value->GetType(),
        type);
    return RValue::Create(*this, type, result);
}

llvm::Value *NJS::Builder::CreateCast(
    const SourceLocation &where,
    const ValueRef &ref,
    const TypePtr &src_type,
    const TypePtr &dst_type) const
{
    const auto &[val_, ptr_] = ref;

    if (src_type == dst_type)
        return val_;

    const auto ty = dst_type->GetLLVM(where, *this);

    if (src_type->IsInteger())
    {
        if (dst_type->IsInteger())
            return GetBuilder().CreateIntCast(val_, ty, dst_type->IsSigned());
        if (dst_type->IsFloatingPoint())
            return src_type->IsSigned()
                       ? GetBuilder().CreateSIToFP(val_, ty)
                       : GetBuilder().CreateUIToFP(val_, ty);
        if (dst_type->IsPointer())
            return GetBuilder().CreateIntToPtr(val_, ty);
    }

    if (src_type->IsFloatingPoint())
    {
        if (dst_type->IsInteger())
            return dst_type->IsSigned()
                       ? GetBuilder().CreateFPToSI(val_, ty)
                       : GetBuilder().CreateFPToUI(val_, ty);
        if (dst_type->IsFloatingPoint())
            return GetBuilder().CreateFPCast(val_, ty);
    }

    if (src_type->IsPointer())
    {
        if (dst_type->IsInteger())
            return GetBuilder().CreatePtrToInt(val_, ty);
        if (dst_type->IsPointer())
            return GetBuilder().CreatePointerCast(val_, ty);
    }

    if (ptr_ && src_type->IsArray())
    {
        if (dst_type->IsPointer() && src_type->GetElement() == dst_type->GetElement())
            return GetBuilder().CreateConstGEP2_64(src_type->GetLLVM(where, *this), ptr_, 0, 0);
    }

    if (src_type->IsFunction())
    {
        if (dst_type->IsFunction())
            return GetBuilder().CreatePointerCast(val_, ty);
    }

    Error(where, "no cast from value of type {} to {}", src_type, dst_type);
}
