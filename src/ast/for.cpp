#include <llvm/IR/BasicBlock.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Value.hpp>

NJS::ForStmt::ForStmt(StmtPtr init, ExprPtr condition, StmtPtr loop, StmtPtr body)
    : Init(std::move(init)), Condition(std::move(condition)), Loop(std::move(loop)), Body(std::move(body))
{
}

NJS::ValuePtr NJS::ForStmt::GenLLVM(Builder& builder)
{
    builder.Push();

    const auto parent = builder.LLVMBuilder().GetInsertBlock()->getParent();
    const auto head = llvm::BasicBlock::Create(builder.LLVMContext(), "head", parent);
    const auto loop = llvm::BasicBlock::Create(builder.LLVMContext(), "loop", parent);
    const auto end = llvm::BasicBlock::Create(builder.LLVMContext(), "end", parent);

    if (Init) Init->GenLLVM(builder);
    builder.LLVMBuilder().CreateBr(head);

    builder.LLVMBuilder().SetInsertPoint(head);
    if (Condition)
    {
        const auto condition = Condition->GenLLVM(builder);
        builder.LLVMBuilder().CreateCondBr(condition->Load(), loop, end);
    }
    else builder.LLVMBuilder().CreateBr(loop);

    builder.LLVMBuilder().SetInsertPoint(loop);
    Body->GenLLVM(builder);
    if (Loop) Loop->GenLLVM(builder);
    builder.LLVMBuilder().CreateBr(head);

    builder.LLVMBuilder().SetInsertPoint(end);
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
