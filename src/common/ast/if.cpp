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

NJS::ValuePtr NJS::IfStatement::GenLLVM(Builder &builder, ErrorInfo &error) const
{
    const auto parent_function = builder.GetBuilder().GetInsertBlock()->getParent();
    auto then_block = llvm::BasicBlock::Create(builder.GetContext(), "then", parent_function);
    auto else_block = ElseBody
                          ? llvm::BasicBlock::Create(builder.GetContext(), "else", parent_function)
                          : nullptr;
    const auto end_block = llvm::BasicBlock::Create(builder.GetContext(), "end", parent_function);

    auto condition = Condition->GenLLVM(builder, error, builder.GetTypeContext().GetBooleanType());
    condition = builder.CreateCast(Condition->Where, condition, builder.GetTypeContext().GetBooleanType());
    const auto condition_value = condition->Load(Condition->Where);

    if (const auto const_condition = llvm::dyn_cast<llvm::ConstantInt>(condition_value))
    {
        if (const_condition->isZero())
        {
            builder.GetBuilder().CreateBr(else_block ? else_block : end_block);
            then_block->eraseFromParent();
            then_block = nullptr;
        }
        else
            builder.GetBuilder().CreateBr(then_block);
    }
    else
        builder.GetBuilder().CreateCondBr(
            condition_value,
            then_block,
            else_block ? else_block : end_block);

    const llvm::Instruction *then_terminator{};
    if (then_block)
    {
        builder.GetBuilder().SetInsertPoint(then_block);
        ThenBody->GenLLVM(builder, error);
        then_block = builder.GetBuilder().GetInsertBlock();
        then_terminator = then_block->getTerminator();
        if (!then_terminator)
            builder.GetBuilder().CreateBr(end_block);
    }

    const llvm::Instruction *else_terminator{};
    if (ElseBody)
    {
        builder.GetBuilder().SetInsertPoint(else_block);
        ElseBody->GenLLVM(builder, error);
        else_block = builder.GetBuilder().GetInsertBlock();
        else_terminator = else_block->getTerminator();
        if (!else_terminator)
            builder.GetBuilder().CreateBr(end_block);
    }

    if (then_terminator && else_terminator)
    {
        end_block->eraseFromParent();
        return {};
    }

    builder.GetBuilder().SetInsertPoint(end_block);
    return {};
}

std::ostream &NJS::IfStatement::Print(std::ostream &stream)
{
    ThenBody->Print(Condition->Print(stream << "if (") << ") ");
    if (ElseBody)
        ElseBody->Print(stream << " else ");
    return stream;
}
