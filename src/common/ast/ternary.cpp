#include <utility>
#include <llvm/IR/BasicBlock.h>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

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

std::ostream &NJS::TernaryExpression::Print(std::ostream &stream) const
{
    if (Condition == ThenBody)
        return ElseBody->Print(ThenBody->Print(stream << '(') << " ?? ") << ')';
    return ElseBody->Print(ThenBody->Print(Condition->Print(stream << '(') << " ? ") << " : ") << ')';
}

NJS::ValuePtr NJS::TernaryExpression::_GenIntermediate(Builder &builder, const TypePtr &expected_type)
{
    const auto parent = builder.GetBuilder().GetInsertBlock()->getParent();
    auto then_block = llvm::BasicBlock::Create(builder.GetContext(), "then", parent);
    auto else_block = llvm::BasicBlock::Create(builder.GetContext(), "else", parent);
    const auto tail_block = llvm::BasicBlock::Create(builder.GetContext(), "tail", parent);

    auto condition = Condition->GenIntermediate(builder, builder.GetTypeContext().GetBooleanType());

    ValuePtr then_value;
    if (Condition == ThenBody)
        then_value = condition;

    if (!condition->GetType()->IsBoolean())
    {
        if (!condition->GetType()->IsIntegerLike())
            Error(Where, "cannot use value of non-integer-like type {} as condition", condition->GetType());
        condition = RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateIsNotNull(condition->Load()));
    }
    builder.GetBuilder().CreateCondBr(condition->Load(), then_block, else_block);

    builder.GetBuilder().SetInsertPoint(then_block);
    if (!then_value)
        then_value = ThenBody->GenIntermediate(builder, expected_type);
    if (then_value->IsLValue())
        then_value = RValue::Create(builder, then_value->GetType(), then_value->Load());
    then_block = builder.GetBuilder().GetInsertBlock();
    const auto then_terminator = builder.GetBuilder().CreateBr(tail_block);

    builder.GetBuilder().SetInsertPoint(else_block);
    auto else_value = ElseBody->GenIntermediate(builder, expected_type);
    if (else_value->IsLValue())
        else_value = RValue::Create(builder, else_value->GetType(), else_value->Load());
    else_block = builder.GetBuilder().GetInsertBlock();
    const auto else_terminator = builder.GetBuilder().CreateBr(tail_block);

    const auto result_type = CombineTypes(
        builder.GetTypeContext(),
        then_value->GetType(),
        else_value->GetType());

    builder.GetBuilder().SetInsertPoint(then_terminator);
    then_value = builder.CreateCast(then_value, result_type);
    builder.GetBuilder().SetInsertPoint(else_terminator);
    else_value = builder.CreateCast(else_value, result_type);

    const auto result_ty = result_type->GetLLVM(builder);

    builder.GetBuilder().SetInsertPoint(tail_block);
    const auto phi_inst = builder.GetBuilder().CreatePHI(result_ty, 2);
    phi_inst->addIncoming(then_value->Load(), then_block);
    phi_inst->addIncoming(else_value->Load(), else_block);

    return RValue::Create(builder, result_type, phi_inst);
}
