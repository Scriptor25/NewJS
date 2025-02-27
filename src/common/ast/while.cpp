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

NJS::ValuePtr NJS::WhileStatement::GenLLVM(Builder &builder) const
{
    builder.StackPush();

    const auto parent_function = builder.GetBuilder().GetInsertBlock()->getParent();
    const auto head_block = llvm::BasicBlock::Create(builder.GetContext(), "head", parent_function);
    auto loop_block = llvm::BasicBlock::Create(builder.GetContext(), "loop", parent_function);
    const auto end_block = llvm::BasicBlock::Create(builder.GetContext(), "end", parent_function);

    builder.GetBuilder().CreateBr(head_block);

    builder.GetBuilder().SetInsertPoint(head_block);
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

    if (loop_block)
    {
        builder.GetBuilder().SetInsertPoint(loop_block);
        Body->GenLLVM(builder);
        builder.GetBuilder().CreateBr(head_block);
    }

    builder.GetBuilder().SetInsertPoint(end_block);

    builder.StackPop();
    return {};
}

std::ostream &NJS::WhileStatement::Print(std::ostream &stream)
{
    return Body->Print(Condition->Print(stream << "while (") << ") ");
}
