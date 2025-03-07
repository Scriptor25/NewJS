#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

NJS::StructExpression::StructExpression(
    SourceLocation where,
    TypePtr type,
    std::vector<std::pair<std::string, ExpressionPtr>> elements)
    : Expression(std::move(where)),
      Type(std::move(type)),
      Elements(std::move(elements))
{
}

NJS::ValuePtr NJS::StructExpression::PGenLLVM(Builder &builder, const TypePtr &expected_type) const
{
    StructTypePtr result_type;
    if (Type)
        result_type = Type::As<StructType>(Type);
    else if (expected_type)
        result_type = Type::As<StructType>(expected_type);

    std::vector<std::pair<std::string, ValuePtr>> element_values;
    std::vector<std::pair<std::string, TypePtr>> element_types;

    for (const auto &[name_, element_]: Elements)
    {
        auto type = result_type
                        ? result_type->GetMember(name_).Info.Type
                        : nullptr;
        auto value = element_->GenLLVM(builder, type);
        element_values.emplace_back(name_, value);
        element_types.emplace_back(name_, value->GetType());
    }

    if (!result_type)
        result_type = builder.GetTypeContext().GetUnsafeStructType(element_types);

    const auto struct_type = result_type->GetLLVM<llvm::StructType>(builder);
    llvm::Value *struct_value = llvm::ConstantStruct::getNullValue(struct_type);
    for (auto &[element_name_, element_value_]: element_values)
    {
        auto [
            index_,
            name_,
            info_
        ] = result_type->GetMember(element_name_);

        element_value_ = builder.CreateCast(element_value_, info_.Type);
        struct_value = builder.GetBuilder().CreateInsertValue(struct_value, element_value_->Load(), index_);
    }

    return RValue::Create(builder, result_type, struct_value);
}

std::ostream &NJS::StructExpression::Print(std::ostream &stream) const
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
