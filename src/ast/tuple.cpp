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

    std::vector<ValuePtr> elements;
    std::vector<TypePtr> element_types;
    auto is_array = true;

    for (unsigned i = 0; i < Elements.size(); ++i)
    {
        const auto type = result_type ? result_type->GetElement(i) : nullptr;
        const auto value = Elements[i]->GenLLVM(builder, type);
        elements.push_back(value);
        element_types.push_back(value->GetType());

        is_array = is_array && element_types.front() == element_types.back();
    }

    if (!result_type)
    {
        if (is_array)
            result_type = builder.GetTypeContext().GetArrayType(element_types.front(), element_types.size());
        else
            result_type = builder.GetTypeContext().GetTupleType(element_types);
    }

    llvm::Value *value = llvm::Constant::getNullValue(result_type->GetLLVM(Where, builder));

    for (unsigned i = 0; i < elements.size(); ++i)
    {
        const auto el = builder.CreateCast(Where, elements[i], result_type->GetElement(i));
        value = builder.GetBuilder().CreateInsertValue(value, el->Load(Where), i);
    }

    return RValue::Create(builder, result_type, value);
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
