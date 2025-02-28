#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

NJS::IfStatement::IfStatement(
    SourceLocation where,
    ExpressionPtr condition,
    StatementPtr then_body,
    StatementPtr else_body)
    : Statement(std::move(where)),
      Condition(std::move(condition)),
      ThenBody(std::move(then_body)),
      ElseBody(std::move(else_body))
{
}

bool NJS::IfStatement::GenLLVM(Builder &builder) const
{
    const auto parent_function = builder.GetBuilder().GetInsertBlock()->getParent();
    auto then_block = llvm::BasicBlock::Create(builder.GetContext(), "then", parent_function);
    auto else_block = ElseBody
                          ? llvm::BasicBlock::Create(builder.GetContext(), "else", parent_function)
                          : nullptr;
    const auto end_block = llvm::BasicBlock::Create(builder.GetContext(), "end", parent_function);

    auto condition = Condition->GenLLVM(builder, builder.GetTypeContext().GetBooleanType());
    if (!condition)
        return true;
    condition = builder.CreateCast(condition, builder.GetTypeContext().GetBooleanType());
    builder.GetBuilder().CreateCondBr(
        condition->Load(),
        then_block,
        else_block ? else_block : end_block);

    const llvm::Instruction *then_terminator{};
    builder.GetBuilder().SetInsertPoint(then_block);
    if (ThenBody->GenLLVM(builder))
        return true;
    then_block = builder.GetBuilder().GetInsertBlock();
    then_terminator = then_block->getTerminator();
    if (!then_terminator)
        builder.GetBuilder().CreateBr(end_block);

    const llvm::Instruction *else_terminator{};
    if (ElseBody)
    {
        builder.GetBuilder().SetInsertPoint(else_block);
        if (ElseBody->GenLLVM(builder))
            return true;
        else_block = builder.GetBuilder().GetInsertBlock();
        else_terminator = else_block->getTerminator();
        if (!else_terminator)
            builder.GetBuilder().CreateBr(end_block);
    }

    if (then_terminator && else_terminator)
    {
        end_block->eraseFromParent();
        return false;
    }

    builder.GetBuilder().SetInsertPoint(end_block);
    return false;
}

std::ostream &NJS::IfStatement::Print(std::ostream &stream)
{
    ThenBody->Print(Condition->Print(stream << "if (") << ") ");
    if (ElseBody)
        ElseBody->Print(stream << " else ");
    return stream;
}
