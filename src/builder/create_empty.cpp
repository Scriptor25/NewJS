#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>

llvm::Value* NJS::Builder::CreateEmpty(const TypePtr& type)
{
    const auto ty = type->GenLLVM(*this);

    if (type->IsPrimitive(Primitive_String))
        return ConstStringExpr::GetString(*this, {});

    if (type->IsPrimitive())
        return llvm::Constant::getNullValue(ty);

    if (type->IsArray() || type->IsTuple() || type->IsObject())
    {
        llvm::Value* value = llvm::Constant::getNullValue(ty);
        for (size_t i = 0; i < type->NumElements(); ++i)
            value = GetBuilder().CreateInsertValue(value, CreateEmpty(type->Element(i)), i);
        return value;
    }

    if (type->IsVector())
    {
        const auto vec_ty = VectorType::GenVecLLVM(*this);
        const auto ptr = CreateMalloc(16);

        const auto gep_ptr = GetBuilder().CreateStructGEP(vec_ty, ptr, 0);
        GetBuilder().CreateStore(llvm::Constant::getNullValue(GetBuilder().getPtrTy()), gep_ptr);

        const auto gep_size = GetBuilder().CreateStructGEP(vec_ty, ptr, 1);
        GetBuilder().CreateStore(llvm::Constant::getNullValue(GetBuilder().getInt64Ty()), gep_size);

        return ptr;
    }

    Error("no empty value for type {}", type);
}
