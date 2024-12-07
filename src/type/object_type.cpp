#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Std.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>

std::string NJS::ObjectType::GenString(const std::map<std::string, TypePtr>& element_types)
{
    std::string str;

    str += "{ ";
    bool first = true;
    for (const auto& [name, type] : element_types)
    {
        if (first) first = false;
        else str += ", ";
        str += name + ": " + type->String;
    }
    return str + " }";
}

NJS::ObjectType::ObjectType(TypeContext& ctx, std::map<std::string, TypePtr> element_types)
    : Type(ctx, GenString(element_types)), ElementTypes(std::move(element_types))
{
}

bool NJS::ObjectType::IsObject() const
{
    return true;
}

NJS::MemberT NJS::ObjectType::Member(const std::string& name) const
{
    if (!ElementTypes.contains(name))
        return {Ctx.GetNoType(), -1};

    size_t i = 0;
    for (const auto& [name_, type_] : ElementTypes)
    {
        if (name_ == name) return {type_, i};
        ++i;
    }

    return {Ctx.GetNoType(), -1};
}

NJS::TypePtr NJS::ObjectType::Element(const size_t i) const
{
    size_t j = 0;
    for (const auto& [name_, type_] : ElementTypes)
        if (j++ >= i) return type_;
    return {};
}

size_t NJS::ObjectType::NumElements() const
{
    return ElementTypes.size();
}

void NJS::ObjectType::TypeInfo(Builder& builder, std::vector<llvm::Value*>& args) const
{
    args.push_back(builder.GetBuilder().getInt32(ID_OBJECT));
    args.push_back(builder.GetBuilder().getInt64(ElementTypes.size()));
    for (const auto& [name_, type_] : ElementTypes)
    {
        args.push_back(ConstStringExpr::GetString(builder, name_));
        type_->TypeInfo(builder, args);
    }
}

size_t NJS::ObjectType::Bytes() const
{
    size_t bytes = 0;
    for (const auto& [name_, type_] : ElementTypes)
        bytes += type_->Bytes();
    return bytes;
}

llvm::Type* NJS::ObjectType::GenLLVM(Builder& builder) const
{
    std::vector<llvm::Type*> elements;
    for (const auto& [name_, type_] : ElementTypes)
        elements.push_back(type_->GenLLVM(builder));
    return llvm::StructType::get(builder.GetContext(), elements);
}
