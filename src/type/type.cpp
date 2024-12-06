#include <NJS/Type.hpp>

NJS::Type::Type(std::string str)
    : String(std::move(str))
{
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

NJS::TypePtr NJS::Type::Member(const std::string&)
{
    return {};
}

size_t NJS::Type::MemberIndex(const std::string&)
{
    return -1;
}

NJS::TypePtr NJS::Type::Result()
{
    return {};
}

NJS::TypePtr NJS::Type::Element()
{
    return {};
}

NJS::TypePtr NJS::Type::Element(size_t)
{
    return {};
}

size_t NJS::Type::NumElements() const
{
    return 0;
}

std::ostream& NJS::Type::Print(std::ostream& os) const
{
    return os << String;
}
