#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::ValuePtr NJS::Builder::CreateVector(const TypePtr& element_type, llvm::Value* data, llvm::Value* size)
{
    const auto ptr = CreateMalloc(16);

    const auto ptr_to_ptr = CreatePtrToVectorPtr(ptr);
    const auto bytes = GetBuilder().CreateMul(size, GetBuilder().getInt64(element_type->Bytes()));
    const auto vec_ptr = CreateMalloc(bytes);
    (void)CreateMemcpy(vec_ptr, data, bytes);
    GetBuilder().CreateStore(vec_ptr, ptr_to_ptr);

    const auto ptr_to_size = CreatePtrToVectorSize(ptr);
    GetBuilder().CreateStore(size, ptr_to_size);

    return RValue::Create(*this, m_Ctx.GetVectorType(element_type), ptr);
}

bool NJS::Builder::CreateVectorStoreArray(const ValuePtr& src, llvm::Value* dst)
{
    const auto type = src->GetType();
    if (!type->IsArray())
        return false;

    const auto ptr_to_size = CreatePtrToVectorSize(dst);
    const auto new_vec_size = GetBuilder().getInt64(type->NumElements());
    GetBuilder().CreateStore(new_vec_size, ptr_to_size);

    const auto ptr_to_ptr = CreatePtrToVectorPtr(dst);
    const auto ptr_ty = GetBuilder().getPtrTy();
    const auto vec_ptr = GetBuilder().CreateLoad(ptr_ty, ptr_to_ptr);
    const auto new_vec_bytes = GetBuilder().getInt64(type->Bytes());
    const auto new_vec_ptr = CreateRealloc(vec_ptr, new_vec_bytes);
    GetBuilder().CreateStore(new_vec_ptr, ptr_to_ptr);

    if (src->IsL())
        (void)CreateMemcpy(new_vec_ptr, src->GetPtr(), new_vec_bytes);
    else
    {
        const auto num_el = type->NumElements();
        const auto el_ty = type->Element()->GenLLVM(*this);
        for (size_t i = 0; i < num_el; ++i)
        {
            const auto dst_ptr = GetBuilder().CreateConstGEP1_64(el_ty, new_vec_ptr, i);
            const auto src_val = GetBuilder().CreateExtractValue(src->Load(), i);
            GetBuilder().CreateStore(src_val, dst_ptr);
        }
    }

    return true;
}

llvm::Value* NJS::Builder::CreatePtrToVectorPtr(llvm::Value* ptr) const
{
    const auto vec_ty = VectorType::GenVecLLVM(*this);
    return GetBuilder().CreateStructGEP(vec_ty, ptr, 0);
}

llvm::Value* NJS::Builder::CreatePtrToVectorSize(llvm::Value* ptr) const
{
    const auto vec_ty = VectorType::GenVecLLVM(*this);
    return GetBuilder().CreateStructGEP(vec_ty, ptr, 1);
}
