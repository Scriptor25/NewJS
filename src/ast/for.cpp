#include <utility>
#include <llvm/IR/BasicBlock.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Value.hpp>

NJS::ForStmt::ForStmt(SourceLocation where, StmtPtr init, ExprPtr condition, StmtPtr loop, StmtPtr body)
    : Stmt(std::move(where)),
      Init(std::move(init)),
      Condition(std::move(condition)),
      Loop(std::move(loop)),
      Body(std::move(body))
{
}

NJS::ValuePtr NJS::ForStmt::GenLLVM(Builder& builder)
{
    builder.Push();

    const auto parent = builder.GetBuilder().GetInsertBlock()->getParent();
    const auto head = llvm::BasicBlock::Create(builder.GetContext(), "head", parent);
    const auto loop = llvm::BasicBlock::Create(builder.GetContext(), "loop", parent);
    const auto end = llvm::BasicBlock::Create(builder.GetContext(), "end", parent);

    if (Init) Init->GenLLVM(builder);
    builder.GetBuilder().CreateBr(head);

    builder.GetBuilder().SetInsertPoint(head);
    if (Condition)
    {
        const auto condition = Condition->GenLLVM(builder);
        builder.GetBuilder().CreateCondBr(condition->Load(), loop, end);
    }
    else builder.GetBuilder().CreateBr(loop);

    builder.GetBuilder().SetInsertPoint(loop);
    Body->GenLLVM(builder);
    if (Loop) Loop->GenLLVM(builder);
    builder.GetBuilder().CreateBr(head);

    builder.GetBuilder().SetInsertPoint(end);
    builder.Pop();
    return {};
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
