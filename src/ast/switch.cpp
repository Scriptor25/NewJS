#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

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

void NJS::SwitchStatement::GenVoidLLVM(Builder &builder) const
{
    const auto parent = builder.GetBuilder().GetInsertBlock()->getParent();
    const auto default_dest = llvm::BasicBlock::Create(builder.GetContext(), "default", parent);
    const auto end_block = llvm::BasicBlock::Create(builder.GetContext(), "end", parent);

    const auto condition = Condition->GenLLVM(builder, {});
    const auto switch_inst = builder.GetBuilder().CreateSwitch(condition->Load(Where), default_dest);

    builder.GetBuilder().SetInsertPoint(default_dest);
    DefaultCase->GenVoidLLVM(builder);
    builder.GetBuilder().CreateBr(end_block);

    for (const auto &[case_, entries_]: Cases)
    {
        const auto dest = llvm::BasicBlock::Create(builder.GetContext(), "case", parent);
        for (const auto &entry: entries_)
        {
            const auto on_val = entry->GenLLVM(builder, condition->GetType());
            const auto on_val_int = llvm::dyn_cast<llvm::ConstantInt>(on_val->Load(Where));
            switch_inst->addCase(on_val_int, dest);
        }
        builder.GetBuilder().SetInsertPoint(dest);
        case_->GenVoidLLVM(builder);
        builder.GetBuilder().CreateBr(end_block);
    }

    builder.GetBuilder().SetInsertPoint(end_block);
}

std::ostream &NJS::SwitchStatement::Print(std::ostream &os)
{
    Condition->Print(os << "switch (") << ") {" << std::endl;
    Indent();
    for (const auto &[case_, entries_]: Cases)
    {
        Spacing(os) << "case ";
        for (unsigned i = 0; i < entries_.size(); ++i)
        {
            if (i > 0)
                os << ", ";
            entries_[i]->Print(os);
        }
        if (const auto p = std::dynamic_pointer_cast<ScopeStatement>(case_))
            case_->Print(os << ' ');
        else
            case_->Print(os << " -> ");
        os << std::endl;
    }
    Spacing(os) << "default";
    if (const auto p = std::dynamic_pointer_cast<ScopeStatement>(DefaultCase))
        DefaultCase->Print(os << ' ');
    else
        DefaultCase->Print(os << " -> ");
    Exdent();
    return Spacing(os << std::endl) << '}';
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

NJS::ValuePtr NJS::SwitchExpression::GenLLVM(Builder &builder, const TypePtr &expected) const
{
    const auto parent = builder.GetBuilder().GetInsertBlock()->getParent();
    auto default_dest = llvm::BasicBlock::Create(builder.GetContext(), "default", parent);
    const auto end_block = llvm::BasicBlock::Create(builder.GetContext(), "end", parent);

    const auto condition = Condition->GenLLVM(builder, {});
    const auto switch_inst = builder.GetBuilder().CreateSwitch(condition->Load(Where), default_dest);

    std::vector<std::pair<llvm::BasicBlock *, ValuePtr>> dest_blocks;
    TypePtr result_type;
    {
        builder.GetBuilder().SetInsertPoint(default_dest);
        auto default_value = DefaultCase->GenLLVM(builder, expected);
        if (default_value->IsLValue())
            default_value = RValue::Create(builder, default_value->GetType(), default_value->Load(Where));
        result_type = expected ? expected : default_value->GetType();
        default_value = builder.CreateCast(Where, default_value, result_type);
        default_dest = builder.GetBuilder().GetInsertBlock();
        dest_blocks.emplace_back(default_dest, default_value);
        builder.GetBuilder().CreateBr(end_block);
    }
    for (const auto &[case_, entries_]: Cases)
    {
        auto dest = llvm::BasicBlock::Create(builder.GetContext(), "case", parent);
        for (const auto &entry: entries_)
        {
            const auto on_val = entry->GenLLVM(builder, condition->GetType());
            const auto on_val_int = llvm::dyn_cast<llvm::ConstantInt>(on_val->Load(Where));
            switch_inst->addCase(on_val_int, dest);
        }
        builder.GetBuilder().SetInsertPoint(dest);
        auto case_value = case_->GenLLVM(builder, result_type);
        if (case_value->IsLValue())
            case_value = RValue::Create(builder, case_value->GetType(), case_value->Load(Where));
        case_value = builder.CreateCast(Where, case_value, result_type);
        dest = builder.GetBuilder().GetInsertBlock();
        dest_blocks.emplace_back(dest, case_value);
        builder.GetBuilder().CreateBr(end_block);
    }

    const auto result_ty = result_type->GetLLVM(Where, builder);

    builder.GetBuilder().SetInsertPoint(end_block);
    const auto phi_inst = builder.GetBuilder().CreatePHI(result_ty, dest_blocks.size());
    for (const auto &[dest_, value_]: dest_blocks)
        phi_inst->addIncoming(value_->Load(Where), dest_);

    return RValue::Create(builder, result_type, phi_inst);
}

std::ostream &NJS::SwitchExpression::Print(std::ostream &os)
{
    Condition->Print(os << "switch (") << ") {" << std::endl;
    Indent();
    for (const auto &[case_, entries_]: Cases)
    {
        Spacing(os) << "case ";
        for (unsigned i = 0; i < entries_.size(); ++i)
        {
            if (i > 0)
                os << ", ";
            entries_[i]->Print(os);
        }
        if (const auto p = std::dynamic_pointer_cast<ScopeExpression>(case_))
            case_->Print(os << ' ');
        else
            case_->Print(os << " -> ");
        os << std::endl;
    }
    Spacing(os) << "default";
    if (const auto p = std::dynamic_pointer_cast<ScopeExpression>(DefaultCase))
        DefaultCase->Print(os << ' ');
    else
        DefaultCase->Print(os << " -> ");
    Exdent();
    return Spacing(os << std::endl) << '}';
}
