#include <NJS/Type.hpp>

NJS::Type::Type(std::string str)
    : String(std::move(str))
{
}

bool NJS::Type::IsComplex() const
{
    return GetId() == TypeId_Complex;
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

llvm::Type* NJS::Type::GenBaseLLVM(Builder&) const
{
    return {};
}

std::ostream& NJS::Type::Print(std::ostream& os) const
{
    return os << String;
}
