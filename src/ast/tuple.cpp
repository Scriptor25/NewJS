#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::TupleExpression::TupleExpression(SourceLocation where, std::vector<ExpressionPtr> elements)
    : Expression(std::move(where)),
      Elements(std::move(elements))
{
}

NJS::ValuePtr NJS::TupleExpression::GenLLVM(Builder &builder, const TypePtr &expected_type) const
{
    std::vector<ValuePtr> elements;
    std::vector<TypePtr> element_types;
    auto is_array = true;
    for (unsigned i = 0; i < Elements.size(); ++i)
    {
        const auto type = expected_type ? expected_type->GetElement(i) : nullptr;
        const auto value = Elements[i]->GenLLVM(builder, type);
        elements.push_back(value);
        element_types.push_back(value->GetType());

        is_array = is_array && element_types.front() == element_types.back();
    }

    TypePtr type;
    if (expected_type)
        type = expected_type;
    else if (is_array)
        type = builder.GetTypeContext().GetArrayType(element_types.front(), element_types.size());
    else
        type = builder.GetTypeContext().GetTupleType(element_types);

    llvm::Value *value = llvm::Constant::getNullValue(type->GetLLVM(Where, builder));

    for (unsigned i = 0; i < elements.size(); ++i)
    {
        const auto el = builder.CreateCast(Where, elements[i], type->GetElement(i));
        value = builder.GetBuilder().CreateInsertValue(value, el->Load(Where), i);
    }

    return RValue::Create(builder, type, value);
}

std::ostream &NJS::TupleExpression::Print(std::ostream &stream)
{
    if (Elements.empty())
        return stream << "[]";

    stream << '[' << std::endl;
    Indent();
    for (const auto &entry: Elements)
        entry->Print(Spacing(stream)) << ',' << std::endl;
    Exdent();
    return Spacing(stream) << ']';
}
