#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>

NJS::TypePtr NJS::max(TypeContext& ctx, const TypePtr& l, const TypePtr& r)
{
    if (l->IsInt())
    {
        if (r->IsInt())
            return ctx.GetIntType(std::max(l->GetBits(), r->GetBits()), l->IsSigned() || r->IsSigned());
        if (r->IsFP())
            return ctx.GetFPType(std::max(l->GetBits(), r->GetBits()));
        Error("TODO");
    }
    if (l->IsFP())
    {
        if (r->IsInt())
            return ctx.GetFPType(std::max(l->GetBits(), r->GetBits()));
        if (r->IsFP())
            return ctx.GetFPType(std::max(l->GetBits(), r->GetBits()));
        Error("TODO");
    }
    Error("TODO");
}

std::ostream& NJS::Type::Print(std::ostream& os) const
{
    return os << m_String;
}

std::string NJS::Type::GetString() const
{
    return m_String;
}

unsigned NJS::Type::GetSize() const
{
    if (m_Size != ~0u) return m_Size;
    return m_Size;
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

bool NJS::Type::IsPointer() const
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
    Error("TODO");
}

unsigned NJS::Type::GetBits() const
{
    Error("TODO");
}

NJS::TypePtr NJS::Type::GetElement() const
{
    Error("TODO");
}

NJS::TypePtr NJS::Type::GetElement(unsigned) const
{
    Error("TODO");
}

NJS::MemberT NJS::Type::GetMember(const std::string&) const
{
    Error("TODO");
}

NJS::TypePtr NJS::Type::GetResult() const
{
    Error("TODO");
}

NJS::Type::Type(TypeContext& ctx, std::string string)
    : m_Ctx(ctx), m_String(std::move(string)), m_LLVM(nullptr), m_Size(~0u)
{
}
