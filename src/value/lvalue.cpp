#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::ValuePtr NJS::LValue::Create(Builder& builder, const TypePtr& type, llvm::Value* ptr)
{
    return std::shared_ptr<LValue>(new LValue(builder, type, ptr));
}

bool NJS::LValue::IsL() const
{
    return true;
}

llvm::Value* NJS::LValue::GetPtr() const
{
    return m_Ptr;
}

llvm::Value* NJS::LValue::Load() const
{
    return GetBuilder().GetBuilder().CreateLoad(GetType()->GenLLVM(GetBuilder()), m_Ptr);
}

void NJS::LValue::Store(llvm::Value* value) const
{
    if (value->getType() != GetType()->GenLLVM(GetBuilder()))
        Error("invalid store: type mismatch, <llvm type> != {}", GetType());
    GetBuilder().GetBuilder().CreateStore(value, m_Ptr);
}

void NJS::LValue::Store(const ValuePtr value) const
{
    if (GetType()->IsVector())
    {
        if (value->GetType()->IsArray())
        {
            const auto vec_ty = VectorType::GenVecLLVM(GetBuilder());

            const auto gep_ptr = GetBuilder().GetBuilder().CreateStructGEP(
                vec_ty,
                Load(),
                0);
            const auto vec_ptr = GetBuilder().GetBuilder().CreateLoad(
                GetBuilder().GetBuilder().getPtrTy(),
                gep_ptr);

            const auto gep_size = GetBuilder().GetBuilder().CreateStructGEP(
                vec_ty,
                Load(),
                1);

            const auto new_vec_size = GetBuilder().GetBuilder().getInt64(value->GetType()->NumElements());
            const auto new_vec_ptr = GetBuilder().CreateRealloc(vec_ptr, new_vec_size);

            GetBuilder().GetBuilder().CreateStore(new_vec_ptr, gep_ptr);
            GetBuilder().GetBuilder().CreateStore(new_vec_size, gep_size);

            if (value->IsL()) GetBuilder().CreateMemcpy(new_vec_ptr, value->GetPtr(), new_vec_size);
            else
            {
                const auto num_el = value->GetType()->NumElements();
                const auto el_ty = value->GetType()->Element()->GenLLVM(GetBuilder());
                for (size_t i = 0; i < num_el; ++i)
                {
                    const auto gep_dst = GetBuilder().GetBuilder().CreateConstGEP1_64(
                        el_ty,
                        new_vec_ptr,
                        i);
                    const auto src = GetBuilder().GetBuilder().CreateExtractValue(
                        value->Load(),
                        i);
                    GetBuilder().GetBuilder().CreateStore(src, gep_dst);
                }
            }
            return;
        }
    }

    if (value->GetType() != GetType())
        Error("invalid store: type mismatch, {} != {}", value->GetType(), GetType());
    GetBuilder().GetBuilder().CreateStore(value->Load(), m_Ptr);
}

NJS::LValue::LValue(Builder& builder, TypePtr type, llvm::Value* ptr)
    : Value(builder, std::move(type)), m_Ptr(ptr)
{
}
