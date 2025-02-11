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

std::string NJS::StackFrame::GetValueName(const std::string_view &name) const
{
    if (name.empty())
        return ParentName;
    if (ParentName.empty())
        return std::string(name);
    return ParentName + '.' + std::string(name);
}
