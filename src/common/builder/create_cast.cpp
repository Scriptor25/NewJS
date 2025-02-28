#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/value.hpp>

NJS::ValuePtr NJS::Builder::CreateCast(const ValuePtr &value, const TypePtr &type)
{
    if (value->GetType() == type)
        return value;

    const auto result = CreateCast(
        {value->Load(), value->IsLValue() ? value->GetPointer() : nullptr},
        value->GetType(),
        type);
    return RValue::Create(*this, type, result);
}

llvm::Value *NJS::Builder::CreateCast(
    const ValueInfo &ref,
    const TypePtr &src_type,
    const TypePtr &dst_type) const
{
    const auto &[val_, ptr_] = ref;

    if (src_type == dst_type)
        return val_;

    const auto ty = dst_type->GetLLVM(*this);

    if (src_type->IsInteger())
    {
        if (dst_type->IsInteger())
            return GetBuilder().CreateIntCast(val_, ty, Type::As<IntegerType>(dst_type)->IsSigned());
        if (dst_type->IsFloatingPoint())
            return Type::As<IntegerType>(src_type)->IsSigned()
                       ? GetBuilder().CreateSIToFP(val_, ty)
                       : GetBuilder().CreateUIToFP(val_, ty);
        if (dst_type->IsPointer())
            return GetBuilder().CreateIntToPtr(val_, ty);
    }

    if (src_type->IsFloatingPoint())
    {
        if (dst_type->IsInteger())
            return Type::As<IntegerType>(dst_type)->IsSigned()
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
        if (dst_type->IsPointer() &&
            Type::As<ArrayType>(src_type)->GetElement() == Type::As<PointerType>(dst_type)->GetElement())
            return GetBuilder().CreateConstGEP2_64(src_type->GetLLVM(*this), ptr_, 0, 0);
    }

    if (src_type->IsFunction())
    {
        if (dst_type->IsFunction())
            return GetBuilder().CreatePointerCast(val_, ty);
    }

    return nullptr;
}
