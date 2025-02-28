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

bool NJS::ForStatement::GenLLVM(Builder &builder) const
{
    const auto parent_function = builder.GetBuilder().GetInsertBlock()->getParent();
    const auto head_block = llvm::BasicBlock::Create(builder.GetContext(), "head", parent_function);
    const auto loop_block = llvm::BasicBlock::Create(builder.GetContext(), "loop", parent_function);
    const auto post_loop_block = Loop
                                     ? llvm::BasicBlock::Create(builder.GetContext(), "post_loop", parent_function)
                                     : head_block;
    const auto end_block = llvm::BasicBlock::Create(builder.GetContext(), "end", parent_function);

    builder.StackPush({}, {}, post_loop_block, end_block);

    if (Initializer && Initializer->GenLLVM(builder))
        return true;

    builder.GetBuilder().CreateBr(head_block);

    builder.GetBuilder().SetInsertPoint(head_block);
    if (Condition)
    {
        auto condition = Condition->GenLLVM(builder, builder.GetTypeContext().GetBooleanType());
        if (!condition)
            return true;
        condition = builder.CreateCast(condition, builder.GetTypeContext().GetBooleanType());
        builder.GetBuilder().CreateCondBr(condition->Load(), loop_block, end_block);
    }
    else
    {
        builder.GetBuilder().CreateBr(loop_block);
    }

    builder.GetBuilder().SetInsertPoint(loop_block);
    if (Body->GenLLVM(builder))
        return true;
    builder.GetBuilder().CreateBr(post_loop_block);

    if (Loop)
    {
        builder.GetBuilder().SetInsertPoint(post_loop_block);
        if (Loop->GenLLVM(builder))
            return true;
        builder.GetBuilder().CreateBr(head_block);
    }

    builder.GetBuilder().SetInsertPoint(end_block);
    builder.StackPop();
    return false;
}

std::ostream &NJS::ForStatement::Print(std::ostream &stream)
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
