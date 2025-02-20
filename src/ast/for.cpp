#include <utility>
#include <llvm/IR/BasicBlock.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

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

void NJS::ForStatement::GenVoidLLVM(Builder &builder) const
{
    builder.StackPush();

    const auto parent_function = builder.GetBuilder().GetInsertBlock()->getParent();
    const auto head_block = llvm::BasicBlock::Create(builder.GetContext(), "head", parent_function);
    auto loop_block = llvm::BasicBlock::Create(builder.GetContext(), "loop", parent_function);
    const auto end_block = llvm::BasicBlock::Create(builder.GetContext(), "end", parent_function);

    if (Initializer)
        Initializer->GenVoidLLVM(builder);
    builder.GetBuilder().CreateBr(head_block);

    builder.GetBuilder().SetInsertPoint(head_block);
    if (Condition)
    {
        auto condition = Condition->GenLLVM(builder, builder.GetTypeContext().GetBooleanType());
        condition = builder.CreateCast(Condition->Where, condition, builder.GetTypeContext().GetBooleanType());

        const auto condition_value = condition->Load(Condition->Where);
        if (const auto const_condition = llvm::dyn_cast<llvm::ConstantInt>(condition_value))
        {
            if (const_condition->isZero())
            {
                builder.GetBuilder().CreateBr(end_block);
                loop_block->eraseFromParent();
                loop_block = nullptr;
            }
            else
                builder.GetBuilder().CreateBr(loop_block);
        }
        else
            builder.GetBuilder().CreateCondBr(condition_value, loop_block, end_block);
    }
    else
        builder.GetBuilder().CreateBr(loop_block);

    if (loop_block)
    {
        builder.GetBuilder().SetInsertPoint(loop_block);
        Body->GenVoidLLVM(builder);
        if (Loop)
            Loop->GenVoidLLVM(builder);
        builder.GetBuilder().CreateBr(head_block);
    }

    builder.GetBuilder().SetInsertPoint(end_block);

    builder.StackPop();
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
