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
        auto type = result_type ? result_type->GetMember(Where, name_).Type : nullptr;
        auto value = element_->GenLLVM(builder, type);
        elements.emplace_back(name_, value);
        element_types.emplace_back(name_, value->GetType());
    }

    if (!result_type)
        result_type = builder.GetTypeContext().GetStructType(element_types);

    llvm::Value *object = llvm::ConstantStruct::getNullValue(result_type->GetLLVM<llvm::StructType>(Where, builder));

    for (unsigned i = 0; i < elements.size(); ++i)
    {
        auto [name_, value_] = elements[i];
        auto [index_, type_] = result_type->GetMember(Where, name_);
        value_ = builder.CreateCast(Where, value_, type_);
        object = builder.GetBuilder().CreateInsertValue(object, value_->Load(Where), index_);
    }

    return RValue::Create(builder, result_type, object);
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
