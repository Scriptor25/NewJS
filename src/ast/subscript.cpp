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
    const auto type = array->GetType();

    const auto index_int = builder.LLVMBuilder().CreateFPToSI(index->Load(), builder.LLVMBuilder().getInt64Ty());

    if (array->IsL())
    {
        const auto ty = type->GenLLVM(builder);
        const auto ptr = array->GetPtr();
        const auto zero = llvm::Constant::getNullValue(index_int->getType());
        const auto gep = builder.LLVMBuilder().CreateInBoundsGEP(ty, ptr, {zero, index_int});
        return LValue::Create(builder, type->Element(), gep);
    }

    const auto idx = index_int;
    const auto const_idx = llvm::dyn_cast<llvm::ConstantInt>(idx);
    const auto value = builder.LLVMBuilder().CreateExtractValue(array->Load(), *const_idx->getValue().getRawData());
    return RValue::Create(builder, type->Element(), value);
}

std::ostream& NJS::SubscriptExpr::Print(std::ostream& os)
{
    return Index->Print(Array->Print(os) << '[') << ']';
}
