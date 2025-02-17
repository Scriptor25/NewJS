#include <ranges>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Std.hpp>
#include <NJS/Type.hpp>

std::string NJS::StructType::GenString(const std::map<std::string, TypePtr> &element_type_map)
{
    std::string dst = "{ ";
    auto first = true;
    for (const auto &[name_, type_]: element_type_map)
    {
        if (first)
            first = false;
        else
            dst += ", ";
        dst += name_ + ": " + type_->GetString();
    }
    return dst += " }";
}

bool NJS::StructType::IsStruct() const
{
    return true;
}

NJS::MemberInfo NJS::StructType::GetMember(const std::string &name) const
{
    unsigned i = 0;
    for (const auto &[name_, type_]: m_ElementTypeMap)
    {
        if (name == name_)
            return {type_, i};
        ++i;
    }
    Error("no member '{}' in {}", name, m_String);
}

void NJS::StructType::TypeInfo(const SourceLocation &where, Builder &builder, std::vector<llvm::Value *> &args) const
{
    args.push_back(builder.GetBuilder().getInt32(ID_STRUCT));
    args.push_back(builder.GetBuilder().getInt32(m_ElementTypeMap.size()));
    for (const auto &[name_, type_]: m_ElementTypeMap)
    {
        args.push_back(StringExpression::GetString(builder, name_));
        type_->TypeInfo(where, builder, args);
    }
}

NJS::StructType::StructType(
    TypeContext &type_context,
    std::string string,
    std::map<std::string, TypePtr> element_type_map)
    : Type(type_context, std::move(string)),
      m_ElementTypeMap(std::move(element_type_map))
{
}

llvm::Type *NJS::StructType::GenLLVM(const SourceLocation &where, const Builder &builder) const
{
    std::vector<llvm::Type *> types;
    for (const auto &type_: m_ElementTypeMap | std::ranges::views::values)
        types.push_back(type_->GetLLVM(where, builder));
    return llvm::StructType::get(builder.GetContext(), types, true);
}

unsigned NJS::StructType::GenSize() const
{
    unsigned size = 0;
    for (const auto &type_: m_ElementTypeMap | std::ranges::views::values)
        size += type_->GetSize();
    return size;
}
