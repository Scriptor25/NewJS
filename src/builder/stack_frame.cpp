#include <NJS/Builder.hpp>

bool NJS::StackFrame::Contains(const std::string &name) const
{
    return Values.contains(name);
}

const NJS::ValuePtr &NJS::StackFrame::operator[](const std::string &name) const
{
    return Values.at(name);
}

NJS::ValuePtr &NJS::StackFrame::operator[](const std::string &name)
{
    return Values[name];
}

std::string NJS::StackFrame::GetChildName(const std::string &name) const
{
    if (name.empty())
        return Name;
    if (Name.empty())
        return name;
    return Name + '.' + name;
}
