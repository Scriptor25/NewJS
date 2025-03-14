#include <utility>
#include <llvm/IR/BasicBlock.h>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

NJS::ForStatement::ForStatement(
    SourceLocation where,
    StatementPtr initializer,
    ExpressionPtr condition,
    StatementPtr loop,
    StatementPtr body)
    : Statement(std::move(where)),
      Initializer(std::move(initializer)),
      Condition(std::move(condition)),
      Loop(std::move(loop)),
      Body(std::move(body))
{
}

std::ostream &NJS::ForStatement::Print(std::ostream &stream) const
{
    stream << "for (";
    if (Initializer)
        Initializer->Print(stream);
    stream << ';';
    if (Condition)
        Condition->Print(stream << ' ');
    stream << ';';
    if (Loop)
        Loop->Print(stream << ' ');
    return Body->Print(stream << ") ");
}

void NJS::ForStatement::PGenLLVM(Builder &builder, bool)
{
    const auto parent_function = builder.GetBuilder().GetInsertBlock()->getParent();
    const auto head_block = llvm::BasicBlock::Create(builder.GetContext(), "head", parent_function);
    const auto loop_block = llvm::BasicBlock::Create(builder.GetContext(), "loop", parent_function);
    const auto post_loop_block = Loop
                                     ? llvm::BasicBlock::Create(builder.GetContext(), "post_loop", parent_function)
                                     : head_block;
    const auto tail_block = llvm::BasicBlock::Create(builder.GetContext(), "tail", parent_function);

    builder.StackPush({}, {}, post_loop_block, tail_block);

    if (Initializer)
        Initializer->GenLLVM(builder, false);

    builder.GetBuilder().CreateBr(head_block);

    builder.GetBuilder().SetInsertPoint(head_block);
    if (Condition)
    {
        auto condition = Condition->GenLLVM(builder, builder.GetTypeContext().GetBooleanType());
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
    }
    else
    {
        builder.GetBuilder().CreateBr(loop_block);
    }

    builder.GetBuilder().SetInsertPoint(loop_block);
    Body->GenLLVM(builder, false);
    builder.GetBuilder().CreateBr(post_loop_block);

    if (Loop)
    {
        builder.GetBuilder().SetInsertPoint(post_loop_block);
        Loop->GenLLVM(builder, false);
        builder.GetBuilder().CreateBr(head_block);
    }

    builder.GetBuilder().SetInsertPoint(tail_block);
    builder.StackPop();
}
