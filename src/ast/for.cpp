#include <utility>
#include <llvm/IR/BasicBlock.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::ForStmt::ForStmt(SourceLocation where, StmtPtr init, ExprPtr condition, StmtPtr loop, StmtPtr body)
    : Stmt(std::move(where)),
      Init(std::move(init)),
      Condition(std::move(condition)),
      Loop(std::move(loop)),
      Body(std::move(body))
{
}

void NJS::ForStmt::GenVoidLLVM(Builder& builder) const
{
    builder.Push();

    const auto parent_function = builder.GetBuilder().GetInsertBlock()->getParent();
    const auto head_block = llvm::BasicBlock::Create(builder.GetContext(), "head", parent_function);
    const auto loop_block = llvm::BasicBlock::Create(builder.GetContext(), "loop", parent_function);
    const auto end_block = llvm::BasicBlock::Create(builder.GetContext(), "end", parent_function);

    if (Init) Init->GenVoidLLVM(builder);
    builder.GetBuilder().CreateBr(head_block);

    builder.GetBuilder().SetInsertPoint(head_block);
    if (Condition)
    {
        const auto condition = Condition->GenLLVM(builder, builder.GetCtx().GetBoolType());
        builder.GetBuilder().CreateCondBr(condition->Load(Where), loop_block, end_block);
    }
    else builder.GetBuilder().CreateBr(loop_block);

    builder.GetBuilder().SetInsertPoint(loop_block);
    Body->GenVoidLLVM(builder);
    if (Loop) Loop->GenVoidLLVM(builder);
    builder.GetBuilder().CreateBr(head_block);

    builder.GetBuilder().SetInsertPoint(end_block);
    builder.Pop();
}

std::ostream& NJS::ForStmt::Print(std::ostream& os)
{
    os << "for (";
    if (Init) Init->Print(os);
    os << ';';
    if (Condition) Condition->Print(os << ' ');
    os << ';';
    if (Loop) Loop->Print(os << ' ');
    return Body->Print(os << ") ");
}
