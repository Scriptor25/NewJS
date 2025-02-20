#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>

NJS::TypePtr NJS::GetHigherOrderOf(
    const SourceLocation &where,
    TypeContext &type_context,
    const TypePtr &type_a,
    const TypePtr &type_b)
{
    if (type_a == type_b)
        return type_a;

    if (type_a->IsInteger())
    {
        if (type_b->IsInteger())
            return type_context.GetIntegerType(
                std::max(type_a->GetBits(where), type_b->GetBits(where)),
                type_a->IsSigned(where) || type_b->IsSigned(where));
        if (type_b->IsFloatingPoint())
            return type_context.GetFloatingPointType(std::max(type_a->GetBits(where), type_b->GetBits(where)));
        if (type_b->IsPointer())
            return type_b;
    }

    if (type_a->IsFloatingPoint())
    {
        if (type_b->IsInteger() || type_b->IsFloatingPoint())
            return type_context.GetFloatingPointType(std::max(type_a->GetBits(where), type_b->GetBits(where)));
    }

    if (type_a->IsPointer())
    {
        if (type_b->IsInteger())
            return type_a;
    }

    return {};
}

std::ostream &NJS::Type::Print(std::ostream &stream) const
{
    return stream << m_String;
}

std::string NJS::Type::GetString() const
{
    return m_String;
}

unsigned NJS::Type::GetSize()
{
    if (m_Size != ~0u)
        return m_Size;
    return m_Size = GenSize();
}

bool NJS::Type::IsPrimitive() const
{
    return false;
}

bool NJS::Type::IsVoid() const
{
    return false;
}

bool NJS::Type::IsInteger() const
{
    return false;
}

bool NJS::Type::IsFloatingPoint() const
{
    return false;
}

bool NJS::Type::IsPointer() const
{
    return false;
}

bool NJS::Type::IsReference() const
{
    return false;
}

bool NJS::Type::IsArray() const
{
    return false;
}

bool NJS::Type::IsStruct() const
{
    return false;
}

bool NJS::Type::IsTuple() const
{
    return false;
}

bool NJS::Type::IsFunction() const
{
    return false;
}

bool NJS::Type::IsSigned(const SourceLocation &where) const
{
    Error(where, "type {} does not support 'IsSigned'", m_String);
}

unsigned NJS::Type::GetBits(const SourceLocation &where) const
{
    Error(where, "type {} does not support 'GetBits'", m_String);
}

NJS::TypePtr NJS::Type::GetElement(const SourceLocation &where) const
{
    Error(where, "type {} does not support 'GetElement'", m_String);
}

NJS::TypePtr NJS::Type::GetElement(const SourceLocation &where, unsigned) const
{
    Error(where, "type {} does not support 'GetElement'", m_String);
}

NJS::MemberInfo NJS::Type::GetMember(const SourceLocation &where, const std::string &) const
{
    Error(where, "type {} does not support 'GetMember'", m_String);
}

NJS::Type::Type(TypeContext &type_context, std::string string)
    : m_TypeContext(type_context),
      m_String(std::move(string)),
      m_LLVM(nullptr),
      m_Size(~0u)
{
}
