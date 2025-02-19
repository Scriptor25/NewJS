#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::WhileStatement::WhileStatement(SourceLocation where, ExpressionPtr condition, StatementPtr body)
    : Statement(std::move(where)),
      Condition(std::move(condition)),
      Body(std::move(body))
{
}

void NJS::WhileStatement::GenVoidLLVM(Builder &builder) const
{
    builder.StackPush();

    const auto parent_function = builder.GetBuilder().GetInsertBlock()->getParent();
    const auto head_block = llvm::BasicBlock::Create(builder.GetContext(), "head", parent_function);
    const auto loop_block = llvm::BasicBlock::Create(builder.GetContext(), "loop", parent_function);
    const auto end_block = llvm::BasicBlock::Create(builder.GetContext(), "end", parent_function);

    builder.GetBuilder().CreateBr(head_block);

    builder.GetBuilder().SetInsertPoint(head_block);
    auto condition = Condition->GenLLVM(builder, builder.GetTypeContext().GetBooleanType());
    condition = builder.CreateCast(Condition->Where, condition, builder.GetTypeContext().GetBooleanType());
    builder.GetBuilder().CreateCondBr(condition->Load(Condition->Where), loop_block, end_block);

    builder.GetBuilder().SetInsertPoint(loop_block);
    Body->GenVoidLLVM(builder);
    builder.GetBuilder().CreateBr(head_block);

    builder.GetBuilder().SetInsertPoint(end_block);

    builder.StackPop();
}

std::ostream &NJS::WhileStatement::Print(std::ostream &stream)
{
    return Body->Print(Condition->Print(stream << "while (") << ") ");
}
