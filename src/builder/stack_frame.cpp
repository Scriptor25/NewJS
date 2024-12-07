#include <NJS/Builder.hpp>

bool NJS::StackFrame::contains(const std::string& name) const
{
    return Values.contains(name);
}

NJS::ValuePtr NJS::StackFrame::operator[](const std::string& name) const
{
    return Values.at(name);
}

NJS::ValuePtr& NJS::StackFrame::operator[](const std::string& name)
{
    return Values[name];
}

std::string NJS::StackFrame::ValueName(const std::string& name) const
{
    if (name.empty()) return ParentName;
    if (ParentName.empty()) return name;
    return ParentName + '.' + name;
}
