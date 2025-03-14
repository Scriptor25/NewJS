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

std::ostream &NJS::IfStatement::Print(std::ostream &stream) const
{
    ThenBody->Print(Condition->Print(stream << "if (") << ") ");
    if (ElseBody)
        ElseBody->Print(stream << " else ");
    return stream;
}

void NJS::IfStatement::PGenLLVM(Builder &builder, bool)
{
    const auto parent_function = builder.GetBuilder().GetInsertBlock()->getParent();
    auto then_block = llvm::BasicBlock::Create(builder.GetContext(), "then", parent_function);
    auto else_block = ElseBody
                          ? llvm::BasicBlock::Create(builder.GetContext(), "else", parent_function)
                          : nullptr;
    const auto tail_block = llvm::BasicBlock::Create(builder.GetContext(), "tail", parent_function);

    auto condition = Condition->GenLLVM(builder, builder.GetTypeContext().GetBooleanType());
    if (!condition->GetType()->IsBoolean())
    {
        if (!condition->GetType()->IsIntegerLike())
            Error(Where, "cannot use value of non-integer-like type {} as condition", condition->GetType());
        condition = RValue::Create(
            builder,
            builder.GetTypeContext().GetBooleanType(),
            builder.GetBuilder().CreateIsNotNull(condition->Load()));
    }
    builder.GetBuilder().CreateCondBr(
        condition->Load(),
        then_block,
        else_block ? else_block : tail_block);

    const llvm::Instruction *then_terminator = nullptr;
    const llvm::Instruction *else_terminator = nullptr;

    builder.GetBuilder().SetInsertPoint(then_block);
    ThenBody->GenLLVM(builder, false);
    then_block = builder.GetBuilder().GetInsertBlock();
    then_terminator = then_block->getTerminator();
    if (!then_terminator)
        builder.GetBuilder().CreateBr(tail_block);

    if (ElseBody)
    {
        builder.GetBuilder().SetInsertPoint(else_block);
        ElseBody->GenLLVM(builder, false);
        else_block = builder.GetBuilder().GetInsertBlock();
        else_terminator = else_block->getTerminator();
        if (!else_terminator)
            builder.GetBuilder().CreateBr(tail_block);
    }

    if (then_terminator && else_terminator)
    {
        tail_block->eraseFromParent();
        return;
    }

    builder.GetBuilder().SetInsertPoint(tail_block);
}
