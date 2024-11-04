#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Value.hpp>

NJS::ConstTupleExpr::ConstTupleExpr(TypePtr type, std::vector<ExprPtr> entries)
    : Expr(std::move(type)), Entries(std::move(entries))
{
}

NJS::ValuePtr NJS::ConstTupleExpr::GenLLVM(Builder& builder)
{
    const auto result = builder.CreateAlloca(Type);

    if (Type->IsTuple())
    {
        const auto llvm_type = Type->GenLLVM(builder);
        const auto ptr = result->GetPtr();
        for (size_t i = 0; i < Entries.size(); ++i)
        {
            const auto value = Entries[i]->GenLLVM(builder);
            const auto gep = builder.LLVMBuilder().CreateStructGEP(llvm_type, ptr, i);
            builder.LLVMBuilder().CreateStore(value->Load(), gep);
        }
    }
    else
    {
        llvm::FunctionCallee malloc;
        builder.GetMalloc(malloc);

        const auto n = builder.LLVMBuilder().getInt64(Entries.size() * Type->ElementSize());
        const auto base = builder.LLVMBuilder().CreateCall(malloc, {n});
        result->Store(base);

        const auto llvm_type = Type->Element()->GenLLVM(builder);
        const auto ptr = result->GetPtr();
        for (size_t i = 0; i < Entries.size(); ++i)
        {
            const auto value = Entries[i]->GenLLVM(builder);
            const auto gep = builder.LLVMBuilder().CreateStructGEP(llvm_type, ptr, i);
            builder.LLVMBuilder().CreateStore(value->Load(), gep);
        }
    }

    return result;
}

std::ostream& NJS::ConstTupleExpr::Print(std::ostream& os)
{
    if (Entries.empty()) return os << "[]";

    os << '[' << std::endl;
    Indent();
    for (const auto& entry : Entries)
        Spacing(os) << entry << ',' << std::endl;
    Exdent();
    return Spacing(os) << ']';
}
