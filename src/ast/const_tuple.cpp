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

    llvm::Value* value = llvm::ConstantStruct::getNullValue(type->GenLLVM(builder));

    if (is_tuple)
    {
        for (size_t i = 0; i < Elements.size(); ++i)
            value = builder.LLVMBuilder().CreateInsertValue(value, values[i]->Load(), {1, i});
    }
    else
    {
        llvm::FunctionCallee malloc;
        builder.GetMalloc(malloc);

        const auto bytes = builder.LLVMBuilder().getInt64(Elements.size() * type->ElementSize());
        const auto malloc_ptr = builder.LLVMBuilder().CreateCall(malloc, {bytes});
        const auto len = builder.LLVMBuilder().getInt64(Elements.size());

        value = builder.LLVMBuilder().CreateInsertValue(value, malloc_ptr, {1, 0});
        value = builder.LLVMBuilder().CreateInsertValue(value, len, {1, 1});

        const auto el_ty = type->Element()->GenLLVM(builder);
        for (size_t i = 0; i < Elements.size(); ++i)
        {
            const auto gep = builder.LLVMBuilder().CreateConstGEP1_64(el_ty, malloc_ptr, i);
            builder.LLVMBuilder().CreateStore(values[i]->Load(), gep);
        }
    }

    return RValue::Create(builder, type, value);
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
