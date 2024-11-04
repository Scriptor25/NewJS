#include <NJS/Type.hpp>

std::ostream& NJS::operator<<(std::ostream& os, const TypePtr& type)
{
    return type->Print(os);
}

NJS::Type::Type(std::string str)
    : String(std::move(str))
{
}

bool NJS::Type::IsTuple()
{
    return false;
}

size_t NJS::Type::Size()
{
    return 0;
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

size_t NJS::Type::ElementSize()
{
    return 0;
}

std::ostream& NJS::Type::Print(std::ostream& os)
{
    return os << String;
}
