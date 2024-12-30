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

NJS::ValuePtr NJS::TupleExpr::GenLLVM(Builder& builder, const TypePtr& expected) const
{
    std::vector<ValuePtr> elements;
    std::vector<TypePtr> element_types;
    bool is_array = true;
    for (unsigned i = 0; i < Elements.size(); ++i)
    {
        const auto type = expected ? expected->GetElement(i) : nullptr;
        const auto value = Elements[i]->GenLLVM(builder, type);
        elements.push_back(value);
        element_types.push_back(value->GetType());

        is_array = is_array && element_types.front() == element_types.back();
    }

    TypePtr type;
    if (expected)
        type = expected;
    else if (is_array)
        type = builder.GetCtx().GetArrayType(element_types.front(), element_types.size());
    else type = builder.GetCtx().GetTupleType(element_types);

    llvm::Value* value = llvm::Constant::getNullValue(type->GetLLVM(Where, builder));

    for (unsigned i = 0; i < elements.size(); ++i)
    {
        const auto el = builder.CreateCast(Where, elements[i], type->GetElement(i));
        value = builder.GetBuilder().CreateInsertValue(value, el->Load(Where), i);
    }

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
