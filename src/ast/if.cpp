#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Value.hpp>

NJS::IfStmt::IfStmt(ExprPtr condition, StmtPtr then, StmtPtr else_)
    : Condition(std::move(condition)), Then(std::move(then)), Else(std::move(else_))
{
}

NJS::ValuePtr NJS::IfStmt::GenLLVM(Builder& builder)
{
    const auto parent = builder.LLVMBuilder().GetInsertBlock()->getParent();
    auto then_bb = llvm::BasicBlock::Create(builder.LLVMContext(), "then", parent);
    auto else_bb = Else
                       ? llvm::BasicBlock::Create(builder.LLVMContext(), "else", parent)
                       : llvm::BasicBlock::Create(builder.LLVMContext(), "end", parent);
    const auto end_bb = Else ? llvm::BasicBlock::Create(builder.LLVMContext(), "end", parent) : else_bb;

    const auto condition = Condition->GenLLVM(builder);
    builder.LLVMBuilder().CreateCondBr(condition->Load(), then_bb, else_bb);

    builder.LLVMBuilder().SetInsertPoint(then_bb);
    Then->GenLLVM(builder);
    then_bb = builder.LLVMBuilder().GetInsertBlock();
    if (!then_bb->getTerminator())
        builder.LLVMBuilder().CreateBr(end_bb);

    if (Else)
    {
        builder.LLVMBuilder().SetInsertPoint(else_bb);
        Else->GenLLVM(builder);
        else_bb = builder.LLVMBuilder().GetInsertBlock();
        if (!else_bb->getTerminator())
            builder.LLVMBuilder().CreateBr(end_bb);
    }

    if (then_bb->getTerminator() && else_bb->getTerminator())
    {
        end_bb->eraseFromParent();
        return {};
    }

    builder.LLVMBuilder().SetInsertPoint(end_bb);
    return {};
}

std::ostream& NJS::IfStmt::Print(std::ostream& os)
{
    os << "if (" << Condition << ") " << Then;
    if (Else) os << " else " << Else;
    return os;
}
