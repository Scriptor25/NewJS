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

    TypePtr type;
    if (is_tuple) type = builder.Ctx().GetTupleType(types);
    else type = builder.Ctx().GetArrayType(types.front(), Elements.size());

    llvm::Value* value = llvm::Constant::getNullValue(type->GenLLVM(builder));
    for (size_t i = 0; i < Elements.size(); ++i)
        value = builder.LLVMBuilder().CreateInsertValue(value, values[i]->Load(), i);

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
