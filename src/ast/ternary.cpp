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
    const auto parent = builder.GetBuilder().GetInsertBlock()->getParent();
    auto then_block = llvm::BasicBlock::Create(builder.GetContext(), "then", parent);
    auto else_block = llvm::BasicBlock::Create(builder.GetContext(), "else", parent);
    const auto end_block = llvm::BasicBlock::Create(builder.GetContext(), "end", parent);

    const auto cond = Condition->GenLLVM(builder);
    builder.GetBuilder().CreateCondBr(cond->Load(), then_block, else_block);

    builder.GetBuilder().SetInsertPoint(then_block);
    auto then_value = Then->GenLLVM(builder);
    if (then_value->IsL())
        then_value = RValue::Create(builder, then_value->GetType(), then_value->Load());
    then_block = builder.GetBuilder().GetInsertBlock();
    builder.GetBuilder().CreateBr(end_block);

    builder.GetBuilder().SetInsertPoint(else_block);
    auto else_value = Else->GenLLVM(builder);
    if (else_value->IsL())
        else_value = RValue::Create(builder, else_value->GetType(), else_value->Load());
    else_block = builder.GetBuilder().GetInsertBlock();
    builder.GetBuilder().CreateBr(end_block);

    if (then_value->GetType() != else_value->GetType())
        Error("invalid ternary operands: type mismatch, {} != {}", then_value->GetType(), else_value->GetType());

    const auto result_type = then_value->GetType();
    const auto result_ty = result_type->GenLLVM(builder);

    builder.GetBuilder().SetInsertPoint(end_block);
    const auto phi_inst = builder.GetBuilder().CreatePHI(result_ty, 2);
    phi_inst->addIncoming(then_value->Load(), then_block);
    phi_inst->addIncoming(else_value->Load(), else_block);

    return RValue::Create(builder, result_type, phi_inst);
}

std::ostream& NJS::TernaryExpr::Print(std::ostream& os)
{
    return Else->Print(Then->Print(Condition->Print(os) << " ? ") << " : ");
}
