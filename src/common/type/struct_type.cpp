#include <ranges>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/std.hpp>
#include <newjs/type.hpp>

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

unsigned NJS::StructType::GetElementCount() const
{
    return m_ElementTypes.size();
}

NJS::MemberInfo NJS::StructType::GetMember(const std::string &name) const
{
    for (unsigned i = 0; i < m_ElementTypes.size(); ++i)
        if (m_ElementTypes[i].first == name)
            return {i, m_ElementTypes[i].first, m_ElementTypes[i].second};
    Error("undefined member '{}' in type {}", name, m_String);
}

NJS::MemberInfo NJS::StructType::GetMember(const unsigned index) const
{
    return {index, m_ElementTypes[index].first, m_ElementTypes[index].second};
}

bool NJS::StructType::TypeInfo(Builder &builder, std::vector<llvm::Value *> &arguments) const
{
    arguments.emplace_back(builder.GetBuilder().getInt32(ID_STRUCT));
    arguments.emplace_back(builder.GetBuilder().getInt32(m_ElementTypes.size()));
    auto any_incomplete = false;
    for (const auto &[name_, type_]: m_ElementTypes)
    {
        arguments.emplace_back(builder.GetString(name_));
        any_incomplete |= type_->TypeInfo(builder, arguments);
    }
    return any_incomplete;
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

llvm::Type *NJS::StructType::GenLLVM(const Builder &builder) const
{
    const auto struct_name = "struct." + std::to_string(m_Index);
    if (const auto struct_type = llvm::StructType::getTypeByName(builder.GetContext(), struct_name))
        return struct_type;

    std::vector<llvm::Type *> types;
    for (const auto &element_type: m_ElementTypes | std::ranges::views::values)
        types.emplace_back(element_type->GetLLVM(builder));

    return llvm::StructType::create(builder.GetContext(), types, struct_name, true);
}
