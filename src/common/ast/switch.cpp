#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/value.hpp>

NJS::SwitchStatement::SwitchStatement(
    SourceLocation where,
    ExpressionPtr condition,
    std::map<StatementPtr, std::vector<ExpressionPtr>> cases,
    StatementPtr default_case)
    : Statement(std::move(where)),
      Condition(std::move(condition)),
      Cases(std::move(cases)),
      DefaultCase(std::move(default_case))
{
}

void NJS::SwitchStatement::PGenLLVM(Builder &builder)
{
    const auto parent = builder.GetBuilder().GetInsertBlock()->getParent();
    const auto tail_block = llvm::BasicBlock::Create(builder.GetContext(), "tail", parent);
    auto default_dest = DefaultCase
                            ? llvm::BasicBlock::Create(builder.GetContext(), "default", parent)
                            : tail_block;

    builder.StackPush({}, {}, {}, tail_block);

    const auto condition = Condition->GenLLVM(builder, {});
    const auto switch_inst = builder.GetBuilder().CreateSwitch(condition->Load(), default_dest);

    if (DefaultCase)
    {
        builder.GetBuilder().SetInsertPoint(default_dest);
        DefaultCase->GenLLVM(builder);
        default_dest = builder.GetBuilder().GetInsertBlock();
        if (!default_dest->getTerminator())
            builder.GetBuilder().CreateBr(tail_block);
    }

    for (const auto &[case_, entries_]: Cases)
    {
        auto dest = llvm::BasicBlock::Create(builder.GetContext(), "case", parent);
        for (const auto &entry: entries_)
        {
            const auto value = entry->GenLLVM(builder, condition->GetType());
            const auto value_int = llvm::dyn_cast<llvm::ConstantInt>(value->Load());
            switch_inst->addCase(value_int, dest);
        }
        builder.GetBuilder().SetInsertPoint(dest);
        case_->GenLLVM(builder);
        dest = builder.GetBuilder().GetInsertBlock();
        if (!dest->getTerminator())
            builder.GetBuilder().CreateBr(tail_block);
    }

    builder.GetBuilder().SetInsertPoint(tail_block);
    builder.StackPop();
}

std::ostream &NJS::SwitchStatement::Print(std::ostream &stream) const
{
    Condition->Print(stream << "switch (") << ") {" << std::endl;
    Indent();
    for (const auto &[case_, entries_]: Cases)
    {
        Spacing(stream) << "case ";
        for (unsigned i = 0; i < entries_.size(); ++i)
        {
            if (i > 0)
                stream << ", ";
            entries_[i]->Print(stream);
        }
        if (const auto p = std::dynamic_pointer_cast<ScopeStatement>(case_))
            case_->Print(stream << ' ');
        else
            case_->Print(stream << " -> ");
        stream << std::endl;
    }
    Spacing(stream) << "default";
    if (const auto p = std::dynamic_pointer_cast<ScopeStatement>(DefaultCase))
        DefaultCase->Print(stream << ' ');
    else
        DefaultCase->Print(stream << " -> ");
    Exdent();
    return Spacing(stream << std::endl) << '}';
}

NJS::SwitchExpression::SwitchExpression(
    SourceLocation where,
    ExpressionPtr condition,
    std::map<ExpressionPtr, std::vector<ExpressionPtr>> cases,
    ExpressionPtr default_case)
    : Expression(std::move(where)),
      Condition(std::move(condition)),
      Cases(std::move(cases)),
      DefaultCase(std::move(default_case))
{
}

NJS::ValuePtr NJS::SwitchExpression::PGenLLVM(Builder &builder, const TypePtr &expected_type)
{
    const auto parent = builder.GetBuilder().GetInsertBlock()->getParent();
    auto default_dest = llvm::BasicBlock::Create(builder.GetContext(), "default", parent);
    const auto tail_block = llvm::BasicBlock::Create(builder.GetContext(), "tail", parent);

    builder.StackPush({}, {}, {}, tail_block);

    const auto condition = Condition->GenLLVM(builder, {});
    const auto switch_inst = builder.GetBuilder().CreateSwitch(condition->Load(), default_dest);

    std::vector<std::pair<llvm::BasicBlock *, ValuePtr>> dest_blocks;
    TypePtr result_type;
    {
        builder.GetBuilder().SetInsertPoint(default_dest);
        auto default_value = DefaultCase->GenLLVM(builder, expected_type);
        if (default_value->IsLValue())
            default_value = RValue::Create(builder, default_value->GetType(), default_value->Load());
        result_type = expected_type ? expected_type : default_value->GetType();
        default_value = builder.CreateCast(default_value, result_type);
        default_dest = builder.GetBuilder().GetInsertBlock();
        dest_blocks.emplace_back(default_dest, default_value);
        if (!default_dest->getTerminator())
            builder.GetBuilder().CreateBr(tail_block);
    }
    for (const auto &[case_, entries_]: Cases)
    {
        auto dest = llvm::BasicBlock::Create(builder.GetContext(), "case", parent);
        for (const auto &entry: entries_)
        {
            const auto value = entry->GenLLVM(builder, condition->GetType());
            const auto value_int = llvm::dyn_cast<llvm::ConstantInt>(value->Load());
            switch_inst->addCase(value_int, dest);
        }
        builder.GetBuilder().SetInsertPoint(dest);
        auto case_value = case_->GenLLVM(builder, result_type);
        if (case_value->IsLValue())
            case_value = RValue::Create(builder, case_value->GetType(), case_value->Load());
        case_value = builder.CreateCast(case_value, result_type);
        dest = builder.GetBuilder().GetInsertBlock();
        dest_blocks.emplace_back(dest, case_value);
        if (!dest->getTerminator())
            builder.GetBuilder().CreateBr(tail_block);
    }

    const auto result_ty = result_type->GetLLVM(builder);

    builder.GetBuilder().SetInsertPoint(tail_block);
    builder.StackPop();

    const auto phi_inst = builder.GetBuilder().CreatePHI(result_ty, dest_blocks.size());
    for (const auto &[dest_, value_]: dest_blocks)
        phi_inst->addIncoming(value_->Load(), dest_);

    return RValue::Create(builder, result_type, phi_inst);
}

std::ostream &NJS::SwitchExpression::Print(std::ostream &stream) const
{
    Condition->Print(stream << "switch (") << ") {" << std::endl;
    Indent();
    for (const auto &[case_, entries_]: Cases)
    {
        Spacing(stream) << "case ";
        for (unsigned i = 0; i < entries_.size(); ++i)
        {
            if (i > 0)
                stream << ", ";
            entries_[i]->Print(stream);
        }
        if (const auto p = std::dynamic_pointer_cast<ScopeExpression>(case_))
            case_->Print(stream << ' ');
        else
            case_->Print(stream << " -> ");
        stream << std::endl;
    }
    Spacing(stream) << "default";
    if (const auto p = std::dynamic_pointer_cast<ScopeExpression>(DefaultCase))
        DefaultCase->Print(stream << ' ');
    else
        DefaultCase->Print(stream << " -> ");
    Exdent();
    return Spacing(stream << std::endl) << '}';
}
