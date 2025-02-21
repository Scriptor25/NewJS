#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::StructExpression::StructExpression(
    SourceLocation where,
    TypePtr type,
    std::vector<std::pair<std::string, ExpressionPtr>> elements)
    : Expression(std::move(where)),
      Type(std::move(type)),
      Elements(std::move(elements))
{
}

NJS::ValuePtr NJS::StructExpression::GenLLVM(Builder &builder, const TypePtr &expected_type) const
{
    TypePtr result_type;
    if (Type)
        result_type = Type;
    else if (expected_type)
        result_type = expected_type;

    std::vector<std::pair<std::string, ValuePtr>> elements;
    std::vector<std::pair<std::string, TypePtr>> element_types;

    for (const auto &[name_, element_]: Elements)
    {
        auto type = result_type ? result_type->GetMember(element_->Where, name_).Type : nullptr;
        auto value = element_->GenLLVM(builder, type);
        elements.emplace_back(name_, value);
        element_types.emplace_back(name_, value->GetType());
    }

    if (!result_type)
        result_type = builder.GetTypeContext().GetStructType(element_types);

    llvm::Value *struct_value = llvm::ConstantStruct::getNullValue(
        result_type->GetLLVM<llvm::StructType>(Where, builder));

    for (unsigned i = 0; i < elements.size(); ++i)
    {
        auto &element = Elements[i].second;
        auto [element_name_, element_value_] = elements[i];
        auto [index_, name_, type_] = result_type->GetMember(element->Where, element_name_);
        element_value_ = builder.CreateCast(element->Where, element_value_, type_);
        struct_value = builder.GetBuilder().CreateInsertValue(
            struct_value,
            element_value_->Load(element->Where),
            index_);
    }

    return RValue::Create(builder, result_type, struct_value);
}

std::ostream &NJS::StructExpression::Print(std::ostream &stream)
{
    if (Elements.empty())
        return stream << "{}";

    stream << '{' << std::endl;
    Indent();
    for (const auto &[name, value]: Elements)
        value->Print(Spacing(stream) << name << ": ") << ',' << std::endl;
    Exdent();
    return Spacing(stream) << '}';
}
