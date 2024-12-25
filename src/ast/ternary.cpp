#include <utility>
#include <llvm/IR/BasicBlock.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::TernaryExpr::TernaryExpr(SourceLocation where, ExprPtr condition, ExprPtr then, ExprPtr else_)
    : Expr(std::move(where)),
      Condition(std::move(condition)),
      Then(std::move(then)),
      Else(std::move(else_))
{
}

NJS::ValuePtr NJS::TernaryExpr::GenLLVM(Builder& builder, const TypePtr& expected)
{
    const auto parent = builder.GetBuilder().GetInsertBlock()->getParent();
    auto then_block = llvm::BasicBlock::Create(builder.GetContext(), "then", parent);
    auto else_block = llvm::BasicBlock::Create(builder.GetContext(), "else", parent);
    const auto end_block = llvm::BasicBlock::Create(builder.GetContext(), "end", parent);

    const auto cond = Condition->GenLLVM(builder, builder.GetCtx().GetBoolType());
    builder.GetBuilder().CreateCondBr(cond->Load(), then_block, else_block);

    builder.GetBuilder().SetInsertPoint(then_block);
    auto then_value = Then->GenLLVM(builder, expected);
    if (then_value->IsL())
        then_value = RValue::Create(builder, then_value->GetType(), then_value->Load());
    then_block = builder.GetBuilder().GetInsertBlock();
    const auto then_term = builder.GetBuilder().CreateBr(end_block);

    builder.GetBuilder().SetInsertPoint(else_block);
    auto else_value = Else->GenLLVM(builder, expected);
    if (else_value->IsL())
        else_value = RValue::Create(builder, else_value->GetType(), else_value->Load());
    else_block = builder.GetBuilder().GetInsertBlock();
    const auto else_term = builder.GetBuilder().CreateBr(end_block);

    const auto result_type = max(builder.GetCtx(), then_value->GetType(), else_value->GetType());

    builder.GetBuilder().SetInsertPoint(then_term);
    then_value = builder.CreateCast(Where, then_value, result_type);
    builder.GetBuilder().SetInsertPoint(else_term);
    else_value = builder.CreateCast(Where, else_value, result_type);

    const auto result_ty = result_type->GetLLVM(builder);

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
