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

std::ostream &NJS::StructExpression::Print(std::ostream &stream) const
{
    if (Elements.empty())
    {
        if (!Type)
            return stream << "({})";
        return Type->Print(stream << "{}:");
    }

    if (!Type)
        stream << '(';
    stream << '{' << std::endl;
    Indent();
    for (const auto &[name_, value_]: Elements)
        value_->Print(Spacing(stream) << name_ << ": ") << ',' << std::endl;
    Exdent();
    Spacing(stream) << '}';
    if (!Type)
        return stream << ')';
    return Type->Print(stream << ':');
}

NJS::ValuePtr NJS::StructExpression::_GenIntermediate(Builder &builder, const TypePtr &expected_type)
{
    StructTypePtr result_type;
    if (Type)
        result_type = Type::As<StructType>(Type);
    else if (expected_type)
        result_type = Type::As<StructType>(expected_type);

    std::vector<std::pair<std::string, TypePtr>> element_types;
    std::map<std::string, ValuePtr> element_values;

    for (const auto &[name_, element_]: Elements)
    {
        auto type = result_type
                        ? result_type->GetMember(name_).Info.Type
                        : nullptr;
        auto value = element_->GenIntermediate(builder, type);
        element_types.emplace_back(name_, value->GetType());
        element_values[name_] = std::move(value);
    }

    if (!result_type)
        result_type = builder.GetTypeContext().GetStructType(element_types, {});

    const auto struct_type = result_type->GetLLVM<llvm::StructType>(builder);
    llvm::Value *struct_value = llvm::ConstantStruct::getNullValue(struct_type);

    for (unsigned i = 0; i < result_type->GetElementCount(); ++i)
    {
        auto [index_, name_, info_, default_] = result_type->GetMember(i);

        ValuePtr element_value;
        if (element_values.contains(name_))
            element_value = element_values[name_];
        else if (default_)
            element_value = default_->GenIntermediate(builder, info_.Type);
        else
            continue;

        const auto value = info_.SolveFor(builder, element_value);

        struct_value = builder.GetBuilder().CreateInsertValue(
            struct_value,
            value,
            index_);
    }

    return RValue::Create(builder, result_type, struct_value);
}
