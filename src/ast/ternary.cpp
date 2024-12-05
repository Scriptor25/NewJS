#include <utility>
#include <llvm/IR/BasicBlock.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::TernaryExpr::TernaryExpr(ExprPtr condition, ExprPtr then, ExprPtr else_)
    : Condition(std::move(condition)), Then(std::move(then)), Else(std::move(else_))
{
}

NJS::ValuePtr NJS::TernaryExpr::GenLLVM(Builder& builder)
{
    const auto parent = builder.LLVMBuilder().GetInsertBlock()->getParent();
    auto then_block = llvm::BasicBlock::Create(builder.LLVMContext(), "then", parent);
    auto else_block = llvm::BasicBlock::Create(builder.LLVMContext(), "else", parent);
    const auto end_block = llvm::BasicBlock::Create(builder.LLVMContext(), "end", parent);

    const auto cond = Condition->GenLLVM(builder);
    builder.LLVMBuilder().CreateCondBr(cond->Load(), then_block, else_block);

    builder.LLVMBuilder().SetInsertPoint(then_block);
    auto then_value = Then->GenLLVM(builder);
    if (then_value->IsL())
        then_value = RValue::Create(builder, then_value->GetType(), then_value->Load());
    then_block = builder.LLVMBuilder().GetInsertBlock();
    builder.LLVMBuilder().CreateBr(end_block);

    builder.LLVMBuilder().SetInsertPoint(else_block);
    auto else_value = Else->GenLLVM(builder);
    if (else_value->IsL())
        else_value = RValue::Create(builder, else_value->GetType(), else_value->Load());
    else_block = builder.LLVMBuilder().GetInsertBlock();
    builder.LLVMBuilder().CreateBr(end_block);

    if (then_value->GetType() != else_value->GetType())
        Error("invalid ternary operands: type mismatch, {} != {}", then_value->GetType(), else_value->GetType());

    const auto result_type = then_value->GetType();
    const auto result_ty = result_type->GenLLVM(builder);

    builder.LLVMBuilder().SetInsertPoint(end_block);
    const auto phi_inst = builder.LLVMBuilder().CreatePHI(result_ty, 2);
    phi_inst->addIncoming(then_value->Load(), then_block);
    phi_inst->addIncoming(else_value->Load(), else_block);

    return RValue::Create(builder, result_type, phi_inst);
}

std::ostream& NJS::TernaryExpr::Print(std::ostream& os)
{
    return Else->Print(Then->Print(Condition->Print(os) << " ? ") << " : ");
}
