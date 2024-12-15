#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>

NJS::Type::Type(TypeContext& ctx, std::string str)
    : Ctx(ctx), String(std::move(str))
{
}

bool NJS::Type::IsNo() const
{
    return false;
}

bool NJS::Type::IsPrimitive() const
{
    return false;
}

bool NJS::Type::IsPrimitive(Primitive) const
{
    return false;
}

bool NJS::Type::IsArray() const
{
    return false;
}

bool NJS::Type::IsTuple() const
{
    return false;
}

bool NJS::Type::IsObject() const
{
    return false;
}

bool NJS::Type::IsFunction() const
{
    return false;
}

bool NJS::Type::IsVector() const
{
    return false;
}

NJS::MemberT NJS::Type::Member(const std::string&) const
{
    return {Ctx.GetNoType(), ~0};
}

NJS::TypePtr NJS::Type::Result() const
{
    return Ctx.GetNoType();
}

NJS::TypePtr NJS::Type::Element() const
{
    return Ctx.GetNoType();
}

NJS::TypePtr NJS::Type::Element(size_t) const
{
    return Ctx.GetNoType();
}

size_t NJS::Type::NumElements() const
{
    return 0;
}

std::ostream& NJS::Type::Print(std::ostream& os) const
{
    return os << String;
}
