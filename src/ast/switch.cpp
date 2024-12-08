#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::SwitchExpr::SwitchExpr(
    SourceLocation where,
    TypePtr type,
    ExprPtr condition,
    std::map<ExprPtr, std::vector<ExprPtr>> cases,
    ExprPtr default_case)
    : Expr(std::move(where), std::move(type)),
      Condition(std::move(condition)),
      Cases(std::move(cases)),
      DefaultCase(std::move(default_case))
{
}

NJS::ValuePtr NJS::SwitchExpr::GenLLVM(Builder& builder)
{
    const auto parent = builder.GetBuilder().GetInsertBlock()->getParent();
    auto default_dest = llvm::BasicBlock::Create(builder.GetContext(), "default", parent);
    const auto end_block = llvm::BasicBlock::Create(builder.GetContext(), "end", parent);

    const auto condition = Condition->GenLLVM(builder);
    if (!condition->GetType()->IsPrimitive(Primitive_Number))
        Error(Where, "invalid switch condition: must be of type number, but is {}", condition->GetType());
    const auto switcher_int = builder.GetBuilder().CreateFPToSI(condition->Load(), builder.GetBuilder().getInt64Ty());
    const auto switch_inst = builder.GetBuilder().CreateSwitch(switcher_int, default_dest);

    std::vector<std::pair<llvm::BasicBlock*, ValuePtr>> dest_blocks;
    TypePtr result_type;
    {
        builder.GetBuilder().SetInsertPoint(default_dest);
        auto default_value = DefaultCase->GenLLVM(builder);
        if (default_value->IsL())
            default_value = RValue::Create(builder, default_value->GetType(), default_value->Load());
        result_type = default_value->GetType();
        default_dest = builder.GetBuilder().GetInsertBlock();
        dest_blocks.emplace_back(default_dest, default_value);
        builder.GetBuilder().CreateBr(end_block);
    }
    for (const auto& [case_, entries_] : Cases)
    {
        auto dest = llvm::BasicBlock::Create(builder.GetContext(), "case", parent);
        for (const auto& entry : entries_)
        {
            const auto on_val = entry->GenLLVM(builder);
            if (!on_val->GetType()->IsPrimitive(Primitive_Number))
                Error(Where, "invalid case entry: must be of type number, but is {}", on_val->GetType());
            const auto on_val_num = llvm::dyn_cast<llvm::ConstantFP>(on_val->Load());
            const auto cast_inst = builder.GetBuilder().CreateFPToSI(on_val_num, builder.GetBuilder().getInt64Ty());
            const auto on_val_int = llvm::dyn_cast<llvm::ConstantInt>(cast_inst);
            switch_inst->addCase(on_val_int, dest);
        }
        builder.GetBuilder().SetInsertPoint(dest);
        auto case_value = case_->GenLLVM(builder);
        if (case_value->IsL())
            case_value = RValue::Create(builder, case_value->GetType(), case_value->Load());
        if (case_value->GetType() != result_type)
            Error(Where, "invalid case value: type mismatch, {} != {}", case_value->GetType(), result_type);
        dest = builder.GetBuilder().GetInsertBlock();
        dest_blocks.emplace_back(dest, case_value);
        builder.GetBuilder().CreateBr(end_block);
    }

    const auto result_ty = result_type->GenLLVM(builder);

    builder.GetBuilder().SetInsertPoint(end_block);
    const auto phi_inst = builder.GetBuilder().CreatePHI(result_ty, dest_blocks.size());
    for (const auto& [dest_, value_] : dest_blocks)
        phi_inst->addIncoming(value_->Load(), dest_);

    return RValue::Create(builder, result_type, phi_inst);
}

std::ostream& NJS::SwitchExpr::Print(std::ostream& os)
{
    Condition->Print(os << "switch (") << ") {" << std::endl;
    Indent();
    for (const auto& [case_, entries_] : Cases)
    {
        Spacing(os) << "case ";
        for (size_t i = 0; i < entries_.size(); ++i)
        {
            if (i > 0) os << ", ";
            entries_[i]->Print(os);
        }
        if (const auto p = std::dynamic_pointer_cast<ScopeExpr>(case_))
            case_->Print(os << ' ');
        else case_->Print(os << " -> ");
        os << std::endl;
    }
    Spacing(os) << "default";
    if (const auto p = std::dynamic_pointer_cast<ScopeExpr>(DefaultCase))
        DefaultCase->Print(os << ' ');
    else DefaultCase->Print(os << " -> ");
    Exdent();
    return Spacing(os << std::endl) << '}';
}
