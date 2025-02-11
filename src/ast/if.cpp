#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::IfStmt::IfStmt(SourceLocation where, ExprPtr condition, StmtPtr then, StmtPtr else_)
    : Stmt(std::move(where)),
      Condition(std::move(condition)),
      Then(std::move(then)),
      Else(std::move(else_))
{
}

void NJS::IfStmt::GenVoidLLVM(Builder &builder) const
{
    const auto parent_function = builder.GetBuilder().GetInsertBlock()->getParent();
    auto then_block = llvm::BasicBlock::Create(builder.GetContext(), "then", parent_function);
    auto else_block = Else
                          ? llvm::BasicBlock::Create(builder.GetContext(), "else", parent_function)
                          : nullptr;
    const auto end_block = llvm::BasicBlock::Create(builder.GetContext(), "end", parent_function);

    const auto condition = Condition->GenLLVM(builder, builder.GetCtx().GetBoolType());
    builder.GetBuilder().CreateCondBr(
        condition->Load(Condition->Where),
        then_block,
        else_block ? else_block : end_block);

    builder.GetBuilder().SetInsertPoint(then_block);
    Then->GenVoidLLVM(builder);
    then_block = builder.GetBuilder().GetInsertBlock();
    const auto then_terminator = then_block->getTerminator();
    if (!then_terminator)
        builder.GetBuilder().CreateBr(end_block);

    const llvm::Instruction *else_terminator{};
    if (Else)
    {
        builder.GetBuilder().SetInsertPoint(else_block);
        Else->GenVoidLLVM(builder);
        else_block = builder.GetBuilder().GetInsertBlock();
        else_terminator = else_block->getTerminator();
        if (!else_terminator)
            builder.GetBuilder().CreateBr(end_block);
    }

    if (then_terminator && else_terminator)
    {
        end_block->eraseFromParent();
        return;
    }

    builder.GetBuilder().SetInsertPoint(end_block);
}

std::ostream &NJS::IfStmt::Print(std::ostream &os)
{
    Then->Print(Condition->Print(os << "if (") << ") ");
    if (Else)
        Else->Print(os << " else ");
    return os;
}
