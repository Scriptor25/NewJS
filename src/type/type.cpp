#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>

NJS::TypePtr NJS::max(TypeContext &ctx, const TypePtr &lhs, const TypePtr &rhs)
{
    if (lhs == rhs)
        return lhs;

    if (lhs->IsInt())
    {
        if (rhs->IsInt())
            return ctx.GetIntType(std::max(lhs->GetBits(), rhs->GetBits()), lhs->IsSigned() || rhs->IsSigned());
        if (rhs->IsFP())
            return ctx.GetFPType(std::max(lhs->GetBits(), rhs->GetBits()));
        if (rhs->IsPtr())
            return rhs;
    }

    if (lhs->IsFP())
    {
        if (rhs->IsInt() || rhs->IsFP())
            return ctx.GetFPType(std::max(lhs->GetBits(), rhs->GetBits()));
    }

    if (lhs->IsPtr())
    {
        if (rhs->IsInt())
            return lhs;
    }

    Error("no maximum type of {} and {}", lhs, rhs);
}

std::ostream &NJS::Type::Print(std::ostream &os) const
{
    return os << m_String;
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

NJS::MemberT NJS::Type::GetMember(const std::string &) const
{
    Error("type {} does not support 'GetMember'", m_String);
}

NJS::TypePtr NJS::Type::GetResult() const
{
    Error("type {} does not support 'GetResult'", m_String);
}

NJS::Type::Type(TypeContext &ctx, std::string string)
    : m_Ctx(ctx),
      m_String(std::move(string)),
      m_LLVM(nullptr),
      m_Size(~0u)
{
}
