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
    for (const auto& element : Elements)
    {
        const auto value = element->GenLLVM(builder);
        values.push_back(value);
        types.push_back(value->GetType());

        if (types.front() != value->GetType())
            is_tuple = true;
    }

    const auto type = is_tuple
                          ? builder.Ctx().GetTupleType(types)
                          : builder.Ctx().GetArrayType(types.front());

    const auto tuple = builder.CreateAlloca(type);

    if (is_tuple)
    {
        const auto llvm_type = type->GenLLVM(builder);
        const auto llvm_ptr = tuple->GetPtr();
        for (size_t i = 0; i < Elements.size(); ++i)
        {
            const auto llvm_gep = builder.LLVMBuilder().CreateStructGEP(llvm_type, llvm_ptr, i);
            builder.LLVMBuilder().CreateStore(values[i]->Load(), llvm_gep);
        }
    }
    else
    {
        llvm::FunctionCallee llvm_malloc;
        builder.GetMalloc(llvm_malloc);

        const auto llvm_bytes = builder.LLVMBuilder().getInt64(Elements.size() * type->ElementSize());
        const auto llvm_ptr = builder.LLVMBuilder().CreateCall(llvm_malloc, {llvm_bytes});

        const auto llvm_array_type = type->GenLLVM(builder);
        {
            const auto llvm_gep = builder.LLVMBuilder().CreateStructGEP(llvm_array_type, tuple->GetPtr(), 0);
            builder.LLVMBuilder().CreateStore(llvm_ptr, llvm_gep);
        }
        {
            const auto llvm_gep = builder.LLVMBuilder().CreateStructGEP(llvm_array_type, tuple->GetPtr(), 1);
            const auto llvm_length = builder.LLVMBuilder().getInt64(Elements.size());
            builder.LLVMBuilder().CreateStore(llvm_length, llvm_gep);
        }

        const auto llvm_element_type = type->Element()->GenLLVM(builder);
        for (size_t i = 0; i < Elements.size(); ++i)
        {
            const auto llvm_gep = builder.LLVMBuilder().CreateStructGEP(llvm_element_type, llvm_ptr, i);
            builder.LLVMBuilder().CreateStore(values[i]->Load(), llvm_gep);
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
