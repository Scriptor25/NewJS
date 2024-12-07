#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::ValuePtr NJS::Builder::CreateSubscript(const ValuePtr& array, const size_t i)
{
    const auto type = array->GetType();

    if (!(type->IsPrimitive(Primitive_String) || type->IsArray() || type->IsTuple() || type->IsVector()))
        Error(
            "invalid subscript: first operand type must be indexable, like string, array, tuple or vector, but is {}",
            type);

    if (type->IsVector())
    {
        const auto ptr = array->Load();
        const auto vec_ty = VectorType::GenVecLLVM(*this);

        const auto gep_ptr = GetBuilder().CreateStructGEP(vec_ty, ptr, 0);
        const auto vec_ptr = GetBuilder().CreateLoad(GetBuilder().getPtrTy(), gep_ptr);

        const auto element_type = type->Element();
        const auto el_ty = element_type->GenLLVM(*this);
        const auto gep = GetBuilder().CreateConstGEP1_64(el_ty, vec_ptr, i);
        return LValue::Create(*this, element_type, gep);
    }

    if (array->GetType()->IsPrimitive(Primitive_String))
    {
        const auto ptr = array->Load();
        const auto gep = GetBuilder().CreateConstGEP1_64(GetBuilder().getInt8Ty(), ptr, i);
        const auto chr = GetBuilder().CreateLoad(GetBuilder().getInt8Ty(), gep);
        return RValue::Create(*this, GetCtx().GetCharType(), chr);
    }

    if (array->IsL())
    {
        const auto ty = type->GenLLVM(*this);
        const auto ptr = array->GetPtr();
        const auto gep = GetBuilder().CreateStructGEP(ty, ptr, i);
        return LValue::Create(*this, type->Element(), gep);
    }

    const auto value = GetBuilder().CreateExtractValue(array->Load(), i);
    return RValue::Create(*this, type->Element(), value);
}

NJS::ValuePtr NJS::Builder::CreateSubscript(const ValuePtr& array, llvm::Value* index)
{
    const auto type = array->GetType();

    if (!(type->IsPrimitive(Primitive_String) || type->IsArray() || type->IsTuple() || type->IsVector()))
        Error(
            "invalid subscript: first operand type must be indexable, like string, array, tuple or vector, but is {}",
            type);

    if (type->IsVector())
    {
        const auto ptr = array->Load();
        const auto vec_ty = VectorType::GenVecLLVM(*this);

        const auto gep_ptr = GetBuilder().CreateStructGEP(vec_ty, ptr, 0);
        const auto vec_ptr = GetBuilder().CreateLoad(GetBuilder().getPtrTy(), gep_ptr);

        const auto element_type = type->Element();
        const auto el_ty = element_type->GenLLVM(*this);
        const auto gep = GetBuilder().CreateGEP(el_ty, vec_ptr, index);
        return LValue::Create(*this, element_type, gep);
    }

    if (array->GetType()->IsPrimitive(Primitive_String))
    {
        const auto ptr = array->Load();
        const auto gep = GetBuilder().CreateGEP(GetBuilder().getInt8Ty(), ptr, index);
        const auto chr = GetBuilder().CreateLoad(GetBuilder().getInt8Ty(), gep);
        return RValue::Create(*this, GetCtx().GetCharType(), chr);
    }

    if (array->IsL())
    {
        const auto ty = type->GenLLVM(*this);
        const auto ptr = array->GetPtr();
        const auto zero = GetBuilder().getInt64(0);
        const auto gep = GetBuilder().CreateInBoundsGEP(ty, ptr, {zero, index});
        return LValue::Create(*this, type->Element(), gep);
    }

    const auto const_idx = llvm::dyn_cast<llvm::ConstantInt>(index);
    const auto value = GetBuilder().CreateExtractValue(array->Load(), *const_idx->getValue().getRawData());
    return RValue::Create(*this, type->Element(), value);
}
