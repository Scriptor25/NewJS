#include <ranges>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/std.hpp>
#include <newjs/type.hpp>

std::string NJS::StructType::GenString(const std::vector<StructElement> &elements)
{
    std::map<std::string, ReferenceInfo> element_map;
    for (auto &[name_, info_, default_]: elements)
        element_map[name_] = info_;

    std::string dst = "{ ";
    auto first = true;
    for (auto &[name_, info_]: element_map)
    {
        if (first)
            first = false;
        else
            dst += ", ";
        dst += name_ + ": " + info_.GetString();
    }
    return dst += " }";
}

bool NJS::StructType::IsStruct() const
{
    return true;
}

unsigned NJS::StructType::GetElementCount() const
{
    return m_Elements.size();
}

NJS::MemberInfo NJS::StructType::GetMember(const std::string &name) const
{
    for (unsigned i = 0; i < m_Elements.size(); ++i)
        if (m_Elements[i].Name == name)
            return {i, m_Elements[i].Name, m_Elements[i].Info, m_Elements[i].Default};
    Error("undefined member '{}' in type {}", name, m_String);
}

NJS::MemberInfo NJS::StructType::GetMember(const unsigned index) const
{
    return {index, m_Elements[index].Name, m_Elements[index].Info, m_Elements[index].Default};
}

bool NJS::StructType::TypeInfo(Builder &builder, std::vector<llvm::Value *> &arguments) const
{
    arguments.emplace_back(builder.GetBuilder().getInt32(ID_STRUCT));
    arguments.emplace_back(builder.GetBuilder().getInt32(m_Elements.size()));
    auto any_incomplete = false;
    for (const auto &[name_, info_, default_]: m_Elements)
    {
        arguments.emplace_back(builder.GetString(name_));
        any_incomplete |= info_.Type->TypeInfo(builder, arguments);
    }
    return any_incomplete;
}

static unsigned struct_count = 0;

NJS::StructType::StructType(
    TypeContext &type_context,
    std::string string,
    std::vector<StructElement> elements)
    : Type(type_context, std::move(string)),
      m_Elements(std::move(elements)),
      m_Index(struct_count++)
{
}

llvm::Type *NJS::StructType::GenLLVM(const Builder &builder) const
{
    const auto struct_name = "struct." + std::to_string(m_Index);
    if (const auto struct_type = llvm::StructType::getTypeByName(builder.GetContext(), struct_name))
        return struct_type;

    std::vector<llvm::Type *> types;
    for (auto &[name_, info_, default_]: m_Elements)
        types.emplace_back(info_.GetLLVM(builder));

    return llvm::StructType::create(builder.GetContext(), types, struct_name, true);
}
