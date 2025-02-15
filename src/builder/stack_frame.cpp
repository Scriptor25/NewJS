#include <NJS/Builder.hpp>

bool NJS::StackFrame::Contains(const std::string_view &name) const
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

std::string NJS::StackFrame::GetChildName(const std::string_view &name) const
{
    if (name.empty())
        return Name;
    if (Name.empty())
        return std::string(name);
    return Name + '.' + std::string(name);
}
