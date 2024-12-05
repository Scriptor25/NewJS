#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::SwitchExpr::SwitchExpr(ExprPtr switcher, std::map<ExprPtr, std::vector<ExprPtr>> cases, ExprPtr default_case)
    : Switcher(std::move(switcher)), Cases(std::move(cases)), DefaultCase(std::move(default_case))
{
}

NJS::ValuePtr NJS::SwitchExpr::GenLLVM(Builder& builder)
{
    const auto parent = builder.LLVMBuilder().GetInsertBlock()->getParent();
    auto default_dest = llvm::BasicBlock::Create(builder.LLVMContext(), "default", parent);
    const auto end_block = llvm::BasicBlock::Create(builder.LLVMContext(), "end", parent);

    const auto switcher = Switcher->GenLLVM(builder);
    const auto switcher_int = builder.LLVMBuilder().CreateFPToSI(switcher->Load(), builder.LLVMBuilder().getInt64Ty());
    const auto switch_inst = builder.LLVMBuilder().CreateSwitch(switcher_int, default_dest);

    std::vector<std::pair<llvm::BasicBlock*, ValuePtr>> dest_blocks;
    for (const auto& [case_, entries_] : Cases)
    {
        auto dest = llvm::BasicBlock::Create(builder.LLVMContext(), "case", parent);
        for (const auto& entry : entries_)
        {
            const auto on_val = entry->GenLLVM(builder);
            const auto on_val_num = llvm::dyn_cast<llvm::ConstantFP>(on_val->Load());
            const auto cast_inst = builder.LLVMBuilder().CreateFPToSI(on_val_num, builder.LLVMBuilder().getInt64Ty());
            const auto on_val_int = llvm::dyn_cast<llvm::ConstantInt>(cast_inst);
            switch_inst->addCase(on_val_int, dest);
        }
        builder.LLVMBuilder().SetInsertPoint(dest);
        auto case_value = case_->GenLLVM(builder);
        if (case_value->IsL())
            case_value = RValue::Create(builder, case_value->GetType(), case_value->Load());
        dest = builder.LLVMBuilder().GetInsertBlock();
        dest_blocks.emplace_back(dest, case_value);
        builder.LLVMBuilder().CreateBr(end_block);
    }
    TypePtr result_type;
    {
        builder.LLVMBuilder().SetInsertPoint(default_dest);
        auto default_value = DefaultCase->GenLLVM(builder);
        if (default_value->IsL())
            default_value = RValue::Create(builder, default_value->GetType(), default_value->Load());
        result_type = default_value->GetType();
        default_dest = builder.LLVMBuilder().GetInsertBlock();
        dest_blocks.emplace_back(default_dest, default_value);
        builder.LLVMBuilder().CreateBr(end_block);
    }

    const auto result_ty = result_type->GenLLVM(builder);

    builder.LLVMBuilder().SetInsertPoint(end_block);
    const auto phi_inst = builder.LLVMBuilder().CreatePHI(result_ty, dest_blocks.size());
    for (const auto& [dest_, value_] : dest_blocks)
        phi_inst->addIncoming(value_->Load(), dest_);

    return RValue::Create(builder, result_type, phi_inst);
}

std::ostream& NJS::SwitchExpr::Print(std::ostream& os)
{
    Switcher->Print(os << "switch (") << ") {" << std::endl;
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
