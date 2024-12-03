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

    const auto ptr = array->GetPtr();
    const auto type = array->GetType();
    const auto ty = type->GenLLVM(builder);
    const auto data_ptr = builder.LLVMBuilder().CreateStructGEP(ty, ptr, 1);
    const auto data_ty = type->GenBaseLLVM(builder);

    const auto ptr_to_base_ptr = builder.LLVMBuilder().CreateStructGEP(data_ty, data_ptr, 0);
    const auto base_ptr = builder.LLVMBuilder().CreateLoad(builder.LLVMBuilder().getPtrTy(), ptr_to_base_ptr);

    const auto el_ty = array->GetType()->Element()->GenLLVM(builder);
    const auto gep = builder.LLVMBuilder().CreateGEP(el_ty, base_ptr, {index->Load()});

    return LValue::Create(builder, array->GetType()->Element(), gep);
}

std::ostream& NJS::SubscriptExpr::Print(std::ostream& os)
{
    return Index->Print(Array->Print(os) << '[') << ']';
}
