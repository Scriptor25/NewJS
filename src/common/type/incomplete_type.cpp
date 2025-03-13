#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/std.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>

std::string NJS::IncompleteType::GenString(const std::string &name)
{
    return name;
}

NJS::TypePtr NJS::IncompleteType::GetBase() const
{
    return GetContext().HasType(m_Name) ? GetContext().GetType(m_Name) : nullptr;
}

size_t NJS::IncompleteType::GetHash() const
{
    return GetContext().HasType(m_Name) ? GetContext().GetType(m_Name)->GetHash() : 0;
}

bool NJS::IncompleteType::IsIncomplete() const
{
    return true;
}

bool NJS::IncompleteType::IsPrimitive() const
{
    if (const auto base = GetBase())
        return base->IsPrimitive();
    return Type::IsPrimitive();
}

bool NJS::IncompleteType::IsVoid() const
{
    if (const auto base = GetBase())
        return base->IsVoid();
    return Type::IsVoid();
}

bool NJS::IncompleteType::IsInteger() const
{
    if (const auto base = GetBase())
        return base->IsInteger();
    return Type::IsInteger();
}

bool NJS::IncompleteType::IsFloatingPoint() const
{
    if (const auto base = GetBase())
        return base->IsFloatingPoint();
    return Type::IsFloatingPoint();
}

bool NJS::IncompleteType::IsPointer() const
{
    if (const auto base = GetBase())
        return base->IsPointer();
    return Type::IsPointer();
}

bool NJS::IncompleteType::IsArray() const
{
    if (const auto base = GetBase())
        return base->IsArray();
    return Type::IsArray();
}

bool NJS::IncompleteType::IsStruct() const
{
    if (const auto base = GetBase())
        return base->IsStruct();
    return Type::IsStruct();
}

bool NJS::IncompleteType::IsTuple() const
{
    if (const auto base = GetBase())
        return base->IsTuple();
    return Type::IsTuple();
}

bool NJS::IncompleteType::IsFunction() const
{
    if (const auto base = GetBase())
        return base->IsFunction();
    return Type::IsFunction();
}

bool NJS::IncompleteType::TypeInfo(
    Builder &builder,
    std::vector<llvm::Value *> &arguments) const
{
    arguments.emplace_back(builder.GetBuilder().getInt32(ID_INCOMPLETE));
    arguments.emplace_back(builder.GetString(m_Name));
    return true;
}

NJS::IncompleteType::IncompleteType(TypeContext &type_context, std::string string, std::string name)
    : Type(type_context, std::move(string)),
      m_Name(std::move(name))
{
}

llvm::Type *NJS::IncompleteType::GenLLVM(const Builder &builder) const
{
    if (GetContext().HasType(m_Name))
        return GetContext().GetType(m_Name)->GetLLVM(builder);

    Error("incomplete type {} does not have a llvm representation", m_Name);
}
