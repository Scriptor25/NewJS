#include <NJS/Builder.hpp>

bool NJS::StackFrame::contains(const std::string_view &name) const
{
    return Values.contains(std::string(name));
}

NJS::ValuePtr NJS::StackFrame::operator[](const std::string_view &name) const
{
    return Values.at(std::string(name));
}

NJS::ValuePtr &NJS::StackFrame::operator[](const std::string_view &name)
{
    return Values[std::string(name)];
}

std::string NJS::StackFrame::ValueName(const std::string_view &name) const
{
    if (name.empty())
        return ParentName;
    if (ParentName.empty())
        return std::string(name);
    return ParentName + '.' + std::string(name);
}
