#include <NJS/Param.hpp>
#include <NJS/Type.hpp>

std::ostream& NJS::operator<<(std::ostream& os, const ParamPtr& ref)
{
    return ref->Print(os);
}

std::ostream& NJS::Param::Print(std::ostream& os)
{
    os << Name;
    if (Type) os << ": " << Type;
    return os;
}

std::ostream& NJS::DestructureObject::Print(std::ostream& os)
{
    os << "{ ";
    bool first = true;
    for (const auto& [key, value] : Elements)
    {
        if (first) first = false;
        else os << ", ";
        os << key << ": " << value;
    }
    os << " }";
    if (Type) os << ": " << Type;
    return os;
}

std::ostream& NJS::DestructureArray::Print(std::ostream& os)
{
    os << "[ ";
    for (size_t i = 0; i < Elements.size(); ++i)
    {
        if (i > 0) os << ", ";
        os << Elements[i];
    }
    os << " ]";
    if (Type) os << ": " << Type;
    return os;
}
