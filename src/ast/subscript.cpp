#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::SubscriptExpr::SubscriptExpr(ExprPtr array, ExprPtr index)
    : Array(std::move(array)), Index(std::move(index))
{
}

NJS::ValuePtr NJS::SubscriptExpr::GenLLVM(Builder& builder)
{
    const auto array = Array->GenLLVM(builder);
    const auto index = Index->GenLLVM(builder);

    const auto llvm_type = array->GetLLVMType();
    const auto llvm_element_type = array->GetType()->Element()->GenLLVM(builder);
    const auto llvm_ptr_to_base_ptr = builder.LLVMBuilder().CreateStructGEP(llvm_type, array->GetPtr(), 0);

    const auto llvm_base_ptr = builder.LLVMBuilder().CreateLoad(builder.LLVMBuilder().getPtrTy(), llvm_ptr_to_base_ptr);
    const auto llvm_gep = builder.LLVMBuilder().CreateGEP(llvm_element_type, llvm_base_ptr, {index->Load()});

    return LValue::Create(builder, array->GetType()->Element(), llvm_gep);
}

std::ostream& NJS::SubscriptExpr::Print(std::ostream& os)
{
    return Index->Print(Array->Print(os) << '[') << ']';
}
