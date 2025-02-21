#include <ranges>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Std.hpp>
#include <NJS/Type.hpp>

std::string NJS::StructType::GenString(const std::vector<std::pair<std::string, TypePtr>> &element_types)
{
    std::map<std::string, TypePtr> element_map;
    for (auto &[name_, type_]: element_types)
        element_map[name_] = type_;

    std::string dst = "{ ";
    auto first = true;
    for (auto &[name_, type_]: element_map)
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

unsigned NJS::StructType::GetElementCount(const SourceLocation &) const
{
    return m_ElementTypes.size();
}

NJS::MemberInfo NJS::StructType::GetMember(const SourceLocation &where, const std::string &name) const
{
    for (unsigned i = 0; i < m_ElementTypes.size(); ++i)
        if (m_ElementTypes[i].first == name)
            return {i, m_ElementTypes[i].first, m_ElementTypes[i].second};
    Error(where, "no member '{}' in type {}", name, m_String);
}

NJS::MemberInfo NJS::StructType::GetMember(const SourceLocation &, const unsigned index) const
{
    return {index, m_ElementTypes[index].first, m_ElementTypes[index].second};
}

void NJS::StructType::TypeInfo(const SourceLocation &where, Builder &builder, std::vector<llvm::Value *> &args) const
{
    args.push_back(builder.GetBuilder().getInt32(ID_STRUCT));
    args.push_back(builder.GetBuilder().getInt32(m_ElementTypes.size()));
    for (const auto &[name_, type_]: m_ElementTypes)
    {
        args.emplace_back(StringExpression::GetString(builder, name_));
        type_->TypeInfo(where, builder, args);
    }
}

static unsigned struct_count = 0;

NJS::StructType::StructType(
    TypeContext &type_context,
    std::string string,
    std::vector<std::pair<std::string, TypePtr>> element_types)
    : Type(type_context, std::move(string)),
      m_ElementTypes(std::move(element_types)),
      m_Index(struct_count++)
{
}

llvm::Type *NJS::StructType::GenLLVM(const SourceLocation &where, const Builder &builder) const
{
    const auto struct_name = "struct." + std::to_string(m_Index);
    if (const auto struct_type = llvm::StructType::getTypeByName(builder.GetContext(), struct_name))
        return struct_type;

    std::vector<llvm::Type *> types;
    for (const auto &type: m_ElementTypes | std::ranges::views::values)
        types.emplace_back(type->GetLLVM(where, builder));

    return llvm::StructType::create(builder.GetContext(), types, struct_name, true);
}

unsigned NJS::StructType::GenSize() const
{
    unsigned size = 0;
    for (const auto &type: m_ElementTypes | std::ranges::views::values)
        size += type->GetSize();
    return size;
}
