#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::TupleExpression::TupleExpression(SourceLocation where, TypePtr type, std::vector<ExpressionPtr> elements)
    : Expression(std::move(where)),
      Type(std::move(type)),
      Elements(std::move(elements))
{
}

NJS::ValuePtr NJS::TupleExpression::GenLLVM(Builder &builder, const TypePtr &expected_type) const
{
    TypePtr result_type;
    if (Type)
        result_type = Type;
    else if (expected_type)
        result_type = expected_type;

    std::vector<ValuePtr> element_values;
    std::vector<TypePtr> element_types;
    auto is_array = true;

    for (unsigned i = 0; i < Elements.size(); ++i)
    {
        const auto type = result_type ? result_type->GetElement(Elements[i]->Where, i) : nullptr;
        auto value = Elements[i]->GenLLVM(builder, type);
        element_values.emplace_back(value);
        element_types.emplace_back(value->GetType());

        is_array = is_array && element_types.front() == element_types.back();
    }

    if (!result_type)
    {
        if (is_array)
            result_type = builder.GetTypeContext().GetArrayType(element_types.front(), element_types.size());
        else
            result_type = builder.GetTypeContext().GetTupleType(element_types);
    }

    llvm::Value *tuple_value = llvm::Constant::getNullValue(result_type->GetLLVM(Where, builder));

    for (unsigned i = 0; i < element_values.size(); ++i)
    {
        auto &element = Elements[i];
        auto &value = element_values[i];
        auto type = result_type->GetElement(element->Where, i);
        value = builder.CreateCast(element->Where, value, type);
        tuple_value = builder.GetBuilder().CreateInsertValue(tuple_value, value->Load(element->Where), i);
    }

    return RValue::Create(builder, result_type, tuple_value);
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
