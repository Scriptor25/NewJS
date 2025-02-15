#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::StructExpression::StructExpression(SourceLocation where, std::map<std::string, ExpressionPtr> elements)
    : Expression(std::move(where)),
      Elements(std::move(elements))
{
}

NJS::ValuePtr NJS::StructExpression::GenLLVM(Builder &builder, const TypePtr &expected_type) const
{
    std::map<std::string, ValuePtr> elements;
    std::map<std::string, TypePtr> element_types;

    for (const auto &[name_, element_]: Elements)
    {
        const auto type = expected_type ? expected_type->GetMember(name_).first : nullptr;
        const auto value = element_->GenLLVM(builder, type);
        elements[name_] = value;
        element_types[name_] = value->GetType();
    }

    const auto type = expected_type ? expected_type : builder.GetTypeContext().GetStructType(element_types);

    llvm::Value *object = llvm::ConstantStruct::getNullValue(type->GetLLVM<llvm::StructType>(Where, builder));

    for (auto [name_, value_]: elements)
    {
        const auto [type_, index_] = type->GetMember(name_);
        value_ = builder.CreateCast(Where, value_, type_);
        object = builder.GetBuilder().CreateInsertValue(object, value_->Load(Where), index_);
    }

    return RValue::Create(builder, type, object);
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
