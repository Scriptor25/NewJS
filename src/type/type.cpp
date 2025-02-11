#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>

NJS::TypePtr NJS::max(TypeContext &type_context, const TypePtr &type_a, const TypePtr &type_b)
{
    if (type_a == type_b)
        return type_a;

    if (type_a->IsInt())
    {
        if (type_b->IsInt())
            return type_context.GetIntType(
                std::max(type_a->GetBits(), type_b->GetBits()),
                type_a->IsSigned() || type_b->IsSigned());
        if (type_b->IsFP())
            return type_context.GetFPType(std::max(type_a->GetBits(), type_b->GetBits()));
        if (type_b->IsPtr())
            return type_b;
    }

    if (type_a->IsFP())
    {
        if (type_b->IsInt() || type_b->IsFP())
            return type_context.GetFPType(std::max(type_a->GetBits(), type_b->GetBits()));
    }

    if (type_a->IsPtr())
    {
        if (type_b->IsInt())
            return type_a;
    }

    Error("no maximum type of {} and {}", type_a, type_b);
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

bool NJS::Type::IsInt() const
{
    return false;
}

bool NJS::Type::IsFP() const
{
    return false;
}

bool NJS::Type::IsPtr() const
{
    return false;
}

bool NJS::Type::IsRef() const
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

bool NJS::Type::IsSigned() const
{
    Error("type {} does not support 'IsSigned'", m_String);
}

unsigned NJS::Type::GetBits() const
{
    Error("type {} does not support 'GetBits'", m_String);
}

NJS::TypePtr NJS::Type::GetElement() const
{
    Error("type {} does not support 'GetElement'", m_String);
}

NJS::TypePtr NJS::Type::GetElement(unsigned) const
{
    Error("type {} does not support 'GetElement'", m_String);
}

NJS::MemberT NJS::Type::GetMember(const std::string_view &) const
{
    Error("type {} does not support 'GetMember'", m_String);
}

NJS::TypePtr NJS::Type::GetResult() const
{
    Error("type {} does not support 'GetResult'", m_String);
}

NJS::Type::Type(TypeContext &type_context, std::string_view string)
    : m_TypeContext(type_context),
      m_String(std::move(string)),
      m_LLVM(nullptr),
      m_Size(~0u)
{
}
