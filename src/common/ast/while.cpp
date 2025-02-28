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

void NJS::WhileStatement::PGenLLVM(Builder &builder) const
{
    const auto parent_function = builder.GetBuilder().GetInsertBlock()->getParent();
    const auto head_block = llvm::BasicBlock::Create(builder.GetContext(), "head", parent_function);
    const auto loop_block = llvm::BasicBlock::Create(builder.GetContext(), "loop", parent_function);
    const auto end_block = llvm::BasicBlock::Create(builder.GetContext(), "end", parent_function);

    builder.StackPush({}, {}, head_block, end_block);
    builder.GetBuilder().CreateBr(head_block);

    builder.GetBuilder().SetInsertPoint(head_block);
    auto condition = Condition->GenLLVM(builder, builder.GetTypeContext().GetBooleanType());
    condition = builder.CreateCast(condition, builder.GetTypeContext().GetBooleanType());
    builder.GetBuilder().CreateCondBr(condition->Load(), loop_block, end_block);

    builder.GetBuilder().SetInsertPoint(loop_block);
    Body->GenLLVM(builder);
    builder.GetBuilder().CreateBr(head_block);

    builder.GetBuilder().SetInsertPoint(end_block);
    builder.StackPop();
}

std::ostream &NJS::WhileStatement::Print(std::ostream &stream)
{
    return Body->Print(Condition->Print(stream << "while (") << ") ");
}
