#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

NJS::TupleExpression::TupleExpression(SourceLocation where, TypePtr type, std::vector<ExpressionPtr> elements)
    : Expression(std::move(where)),
      Type(std::move(type)),
      Elements(std::move(elements))
{
}

NJS::ValuePtr NJS::TupleExpression::PGenLLVM(Builder &builder, const TypePtr &expected_type) const
{
    TypePtr result_type;
    if (Type)
        result_type = Type;
    else if (expected_type)
    {
        if (!expected_type->IsArray() && !expected_type->IsTuple())
            Error(
                Where,
                "invalid expected type for tuple or array expression, must be tuple- or array-like, but is {}",
                expected_type);
        result_type = expected_type;
    }

    const auto get_element_type = [&](const unsigned index)
    {
        return result_type
                   ? result_type->IsArray()
                         ? Type::As<ArrayType>(result_type)->GetElement()
                         : result_type->IsTuple()
                               ? Type::As<TupleType>(result_type)->GetElement(index)
                               : nullptr
                   : nullptr;
    };

    std::vector<ValuePtr> element_values;
    std::vector<TypePtr> element_types;
    auto is_array = true;

    for (unsigned i = 0; i < Elements.size(); ++i)
    {
        auto type = get_element_type(i);
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

    llvm::Value *tuple_value = llvm::Constant::getNullValue(result_type->GetLLVM(builder));

    for (unsigned i = 0; i < element_values.size(); ++i)
    {
        auto value = element_values[i];
        auto type = get_element_type(i);
        value = builder.CreateCast(value, type);
        tuple_value = builder.GetBuilder().CreateInsertValue(tuple_value, value->Load(), i);
    }

    return RValue::Create(builder, result_type, tuple_value);
}

std::ostream &NJS::TupleExpression::Print(std::ostream &stream) const
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
