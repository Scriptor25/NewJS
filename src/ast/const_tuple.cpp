#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Context.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::ConstTupleExpr::ConstTupleExpr(std::vector<ExprPtr> elements)
    : Elements(std::move(elements))
{
}

NJS::ValuePtr NJS::ConstTupleExpr::GenLLVM(Builder& builder)
{
    std::vector<ValuePtr> values;
    std::vector<TypePtr> types;

    bool is_tuple = false;
    TypePtr first;
    for (const auto& element : Elements)
    {
        const auto value = element->GenLLVM(builder);
        values.push_back(value);
        types.push_back(value->GetType());

        if (!first) first = value->GetType();
        else if (first != value->GetType())
            is_tuple = true;
    }

    const auto type = is_tuple
                          ? builder.Ctx().GetTupleType(types)
                          : builder.Ctx().GetArrayType(first);

    const auto tuple = builder.CreateAlloca(type);

    if (is_tuple)
    {
        const auto llvm_type = type->GenLLVM(builder);
        const auto ptr = tuple->GetPtr();
        for (size_t i = 0; i < Elements.size(); ++i)
        {
            const auto gep = builder.LLVMBuilder().CreateStructGEP(llvm_type, ptr, i);
            builder.LLVMBuilder().CreateStore(values[i]->Load(), gep);
        }
    }
    else
    {
        llvm::FunctionCallee malloc;
        builder.GetMalloc(malloc);

        const auto bytes = builder.LLVMBuilder().getInt64(Elements.size() * type->ElementSize());
        const auto ptr = builder.LLVMBuilder().CreateCall(malloc, {bytes});

        const auto llvm_type = type->GenLLVM(builder);
        {
            const auto gep = builder.LLVMBuilder().CreateStructGEP(llvm_type, tuple->GetPtr(), 0);
            builder.LLVMBuilder().CreateStore(ptr, gep);
        }
        {
            const auto gep = builder.LLVMBuilder().CreateStructGEP(llvm_type, tuple->GetPtr(), 1);
            const auto length = builder.LLVMBuilder().getInt64(Elements.size());
            builder.LLVMBuilder().CreateStore(length, gep);
        }

        const auto e_ty = type->Element()->GenLLVM(builder);
        for (size_t i = 0; i < Elements.size(); ++i)
        {
            const auto gep = builder.LLVMBuilder().CreateStructGEP(e_ty, ptr, i);
            builder.LLVMBuilder().CreateStore(values[i]->Load(), gep);
        }
    }

    return tuple;
}

std::ostream& NJS::ConstTupleExpr::Print(std::ostream& os)
{
    if (Elements.empty()) return os << "[]";

    os << '[' << std::endl;
    Indent();
    for (const auto& entry : Elements)
        entry->Print(Spacing(os)) << ',' << std::endl;
    Exdent();
    return Spacing(os) << ']';
}
