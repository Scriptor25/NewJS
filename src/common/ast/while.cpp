#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

NJS::WhileStatement::WhileStatement(SourceLocation where, ExpressionPtr condition, StatementPtr body)
    : Statement(std::move(where)),
      Condition(std::move(condition)),
      Body(std::move(body))
{
}

std::ostream &NJS::WhileStatement::Print(std::ostream &stream) const
{
    return Body->Print(Condition->Print(stream << "while (") << ") ");
}

void NJS::WhileStatement::_GenIntermediate(Builder &builder, bool)
{
    const auto parent_function = builder.GetBuilder().GetInsertBlock()->getParent();
    const auto head_block = llvm::BasicBlock::Create(builder.GetContext(), "head", parent_function);
    const auto loop_block = llvm::BasicBlock::Create(builder.GetContext(), "loop", parent_function);
    const auto tail_block = llvm::BasicBlock::Create(builder.GetContext(), "tail", parent_function);

    builder.StackPush({}, {}, head_block, tail_block);
    builder.GetBuilder().CreateBr(head_block);

    builder.GetBuilder().SetInsertPoint(head_block);
    auto condition = Condition->GenIntermediate(builder, builder.GetTypeContext().GetBooleanType());
    if (!condition->GetType()->IsBoolean())
    {
        if (!condition->GetType()->IsIntegerLike())
            Error(Where, "cannot use value of non-integer-like type {} as condition", condition->GetType());
        condition = RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateIsNotNull(condition->Load()));
    }
    builder.GetBuilder().CreateCondBr(condition->Load(), loop_block, tail_block);

    builder.GetBuilder().SetInsertPoint(loop_block);
    Body->GenIntermediate(builder, false);
    builder.GetBuilder().CreateBr(head_block);

    builder.GetBuilder().SetInsertPoint(tail_block);
    builder.StackPop();
}
