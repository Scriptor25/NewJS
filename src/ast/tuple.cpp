#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::TupleExpr::TupleExpr(SourceLocation where, std::vector<ExprPtr> elements)
    : Expr(std::move(where)), Elements(std::move(elements))
{
}

NJS::ValuePtr NJS::TupleExpr::GenLLVM(Builder& builder)
{
    std::vector<ValuePtr> elements;
    std::vector<TypePtr> element_types;
    bool is_array = true;
    for (const auto& element : Elements)
    {
        const auto value = element->GenLLVM(builder);
        elements.push_back(value);
        element_types.push_back(value->GetType());

        is_array = is_array && element_types.front() == element_types.back();
    }

    TypePtr type;
    if (is_array)
        type = builder.GetCtx().GetArrayType(element_types.front(), element_types.size());
    else type = builder.GetCtx().GetTupleType(element_types);

    llvm::Value* value = llvm::Constant::getNullValue(type->GetLLVM(builder));

    for (unsigned i = 0; i < elements.size(); ++i)
        value = builder.GetBuilder().CreateInsertValue(value, elements[i]->Load(), i);

    return RValue::Create(builder, type, value);
}

std::ostream& NJS::TupleExpr::Print(std::ostream& os)
{
    if (Elements.empty()) return os << "[]";

    os << '[' << std::endl;
    Indent();
    for (const auto& entry : Elements)
        entry->Print(Spacing(os)) << ',' << std::endl;
    Exdent();
    return Spacing(os) << ']';
}
