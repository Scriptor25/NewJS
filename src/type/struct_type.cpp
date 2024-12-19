#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Std.hpp>
#include <NJS/Type.hpp>

std::string NJS::StructType::GenString(const std::map<std::string, TypePtr>& elements)
{
    std::string dst = "{ ";
    bool first = true;
    for (const auto& [name_, type_] : elements)
    {
        if (first) first = false;
        else dst += ", ";
        dst += name_ + ": " + type_->GetString();
    }
    return dst += " }";
}

bool NJS::StructType::IsStruct() const
{
    return true;
}

NJS::MemberT NJS::StructType::GetMember(const std::string& name) const
{
    unsigned i = 0;
    for (const auto& [name_, type_] : m_Elements)
    {
        if (name == name_)
            return {type_, i};
        ++i;
    }
    Error("no member '{}' in {}", name, m_String);
}

void NJS::StructType::TypeInfo(Builder& builder, std::vector<llvm::Value*>& args) const
{
    args.push_back(builder.GetBuilder().getInt32(ID_STRUCT));
    args.push_back(builder.GetBuilder().getInt32(m_Elements.size()));
    for (const auto& [name_, type_] : m_Elements)
    {
        args.push_back(StringExpr::GetString(builder, name_));
        type_->TypeInfo(builder, args);
    }
}

NJS::StructType::StructType(
    TypeContext& ctx,
    std::string string,
    std::map<std::string, TypePtr> elements)
    : Type(ctx, std::move(string)), m_Elements(std::move(elements))
{
}

llvm::Type* NJS::StructType::GenLLVM(const Builder& builder) const
{
    std::vector<llvm::Type*> types;
    for (const auto& [name_, type_] : m_Elements)
        types.push_back(type_->GetLLVM(builder));
    return llvm::StructType::get(builder.GetContext(), types, true);
}

unsigned NJS::StructType::GenSize() const
{
    unsigned size = 0;
    for (const auto& [name_, type_] : m_Elements)
        size += type_->GetSize();
    return size;
}
