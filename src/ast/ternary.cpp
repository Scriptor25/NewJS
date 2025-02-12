#include <utility>
#include <llvm/IR/BasicBlock.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::TernaryExpression::TernaryExpression(
    SourceLocation where,
    ExpressionPtr condition,
    ExpressionPtr then_body,
    ExpressionPtr else_body)
    : Expression(std::move(where)),
      Condition(std::move(condition)),
      ThenBody(std::move(then_body)),
      ElseBody(std::move(else_body))
{
}

NJS::ValuePtr NJS::TernaryExpression::GenLLVM(Builder &builder, const TypePtr &expected) const
{
    const auto parent = builder.GetBuilder().GetInsertBlock()->getParent();
    auto then_block = llvm::BasicBlock::Create(builder.GetContext(), "then", parent);
    auto else_block = llvm::BasicBlock::Create(builder.GetContext(), "else", parent);
    const auto end_block = llvm::BasicBlock::Create(builder.GetContext(), "end", parent);

    const auto cond = Condition->GenLLVM(builder, builder.GetTypeContext().GetBoolType());
    builder.GetBuilder().CreateCondBr(cond->Load(Where), then_block, else_block);

    builder.GetBuilder().SetInsertPoint(then_block);
    auto then_value = ThenBody->GenLLVM(builder, expected);
    if (then_value->IsL())
        then_value = RValue::Create(builder, then_value->GetType(), then_value->Load(Where));
    then_block = builder.GetBuilder().GetInsertBlock();
    const auto then_term = builder.GetBuilder().CreateBr(end_block);

    builder.GetBuilder().SetInsertPoint(else_block);
    auto else_value = ElseBody->GenLLVM(builder, expected);
    if (else_value->IsL())
        else_value = RValue::Create(builder, else_value->GetType(), else_value->Load(Where));
    else_block = builder.GetBuilder().GetInsertBlock();
    const auto else_term = builder.GetBuilder().CreateBr(end_block);

    const auto result_type = max(builder.GetTypeContext(), then_value->GetType(), else_value->GetType());

    builder.GetBuilder().SetInsertPoint(then_term);
    then_value = builder.CreateCast(Where, then_value, result_type);
    builder.GetBuilder().SetInsertPoint(else_term);
    else_value = builder.CreateCast(Where, else_value, result_type);

    const auto result_ty = result_type->GetLLVM(Where, builder);

    builder.GetBuilder().SetInsertPoint(end_block);
    const auto phi_inst = builder.GetBuilder().CreatePHI(result_ty, 2);
    phi_inst->addIncoming(then_value->Load(Where), then_block);
    phi_inst->addIncoming(else_value->Load(Where), else_block);

    return RValue::Create(builder, result_type, phi_inst);
}

std::ostream &NJS::TernaryExpression::Print(std::ostream &os)
{
    return ElseBody->Print(ThenBody->Print(Condition->Print(os) << " ? ") << " : ");
}
