#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Value.hpp>

NJS::IfStmt::IfStmt(SourceLocation where, ExprPtr condition, StmtPtr then, StmtPtr else_)
    : Stmt(std::move(where)), Condition(std::move(condition)), Then(std::move(then)), Else(std::move(else_))
{
}

NJS::ValuePtr NJS::IfStmt::GenLLVM(Builder& builder)
{
    const auto parent = builder.GetBuilder().GetInsertBlock()->getParent();
    auto then_bb = llvm::BasicBlock::Create(builder.GetContext(), "then", parent);
    auto else_bb = Else
                       ? llvm::BasicBlock::Create(builder.GetContext(), "else", parent)
                       : nullptr;
    const auto end_bb = llvm::BasicBlock::Create(builder.GetContext(), "end", parent);

    const auto condition = Condition->GenLLVM(builder);
    builder.GetBuilder().CreateCondBr(condition->Load(), then_bb, else_bb ? else_bb : end_bb);

    builder.GetBuilder().SetInsertPoint(then_bb);
    Then->GenLLVM(builder);
    then_bb = builder.GetBuilder().GetInsertBlock();
    const auto then_terminator = then_bb->getTerminator();
    if (!then_terminator)
        builder.GetBuilder().CreateBr(end_bb);

    llvm::Instruction* else_terminator{};
    if (Else)
    {
        builder.GetBuilder().SetInsertPoint(else_bb);
        Else->GenLLVM(builder);
        else_bb = builder.GetBuilder().GetInsertBlock();
        else_terminator = else_bb->getTerminator();
        if (!else_terminator)
            builder.GetBuilder().CreateBr(end_bb);
    }

    if (then_terminator && else_terminator)
    {
        end_bb->eraseFromParent();
        return {};
    }

    builder.GetBuilder().SetInsertPoint(end_bb);
    return {};
}

std::ostream& NJS::IfStmt::Print(std::ostream& os)
{
    Then->Print(Condition->Print(os << "if (") << ") ");
    if (Else) Else->Print(os << " else ");
    return os;
}
