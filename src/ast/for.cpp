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
    const auto loop_block = llvm::BasicBlock::Create(builder.GetContext(), "loop", parent_function);
    const auto end_block = llvm::BasicBlock::Create(builder.GetContext(), "end", parent_function);

    if (Initializer)
        Initializer->GenVoidLLVM(builder);
    builder.GetBuilder().CreateBr(head_block);

    builder.GetBuilder().SetInsertPoint(head_block);
    if (Condition)
    {
        const auto condition = Condition->GenLLVM(builder, builder.GetTypeContext().GetBoolType());
        builder.GetBuilder().CreateCondBr(condition->Load(Where), loop_block, end_block);
    }
    else
        builder.GetBuilder().CreateBr(loop_block);

    builder.GetBuilder().SetInsertPoint(loop_block);
    Body->GenVoidLLVM(builder);
    if (Loop)
        Loop->GenVoidLLVM(builder);
    builder.GetBuilder().CreateBr(head_block);

    builder.GetBuilder().SetInsertPoint(end_block);
    builder.StackPop();
}

std::ostream &NJS::ForStatement::Print(std::ostream &os)
{
    os << "for (";
    if (Initializer)
        Initializer->Print(os);
    os << ';';
    if (Condition)
        Condition->Print(os << ' ');
    os << ';';
    if (Loop)
        Loop->Print(os << ' ');
    return Body->Print(os << ") ");
}
