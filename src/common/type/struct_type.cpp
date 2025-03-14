#include <ranges>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/std.hpp>
#include <newjs/type.hpp>

std::string NJS::StructType::GenString(const std::vector<StructElement> &elements, const std::string &name)
{
    if (elements.empty())
        return "{}" + (name.empty() ? "" : '.' + name);

    std::string dst = "{ ";
    for (unsigned i = 0; i < elements.size(); ++i)
    {
        if (i > 0)
            dst += ", ";

        auto &[name_, info_, default_] = elements[i];

        if (info_.IsConst)
            dst += "const ";
        if (info_.IsReference)
            dst += "&";

        dst += name_ + ": " + info_.Type->GetString();
    }
    return dst + " }" + (name.empty() ? "" : '.' + name);
}

unsigned NJS::StructType::GenHash(const std::vector<StructElement> &elements, const std::string &name)
{
    unsigned hash = 0x06;
    if (!name.empty())
        return CombineHashes(hash, std::hash<std::string>()(name));
    for (auto &element: elements)
        hash = CombineHashes(hash, element.Info.GetHash());
    return hash;
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

void NJS::StructType::SetElements(const std::vector<StructElement> &elements)
{
    m_Elements = elements;
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

std::ostream &NJS::StructType::Print(std::ostream &stream) const
{
    return stream << "{}." << m_Name;
}

NJS::StructType::StructType(
    TypeContext &type_context,
    const unsigned hash,
    std::string string,
    std::vector<StructElement> elements,
    std::string name)
    : Type(type_context, hash, std::move(string)),
      m_Elements(std::move(elements)),
      m_Name(std::move(name))
{
}

llvm::Type *NJS::StructType::GenLLVM(const Builder &builder) const
{
    const auto struct_name = "struct." + m_Name;
    if (const auto struct_type = llvm::StructType::getTypeByName(builder.GetContext(), struct_name))
        return struct_type;

    std::vector<llvm::Type *> types;
    for (auto &[name_, info_, default_]: m_Elements)
        types.emplace_back(info_.GetLLVM(builder));

    return llvm::StructType::create(builder.GetContext(), types, struct_name, true);
}
