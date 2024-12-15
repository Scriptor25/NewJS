#include <ostream>
#include <NJS/NJS.hpp>
#include <NJS/Type.hpp>

unsigned depth = 0;

void NJS::Indent()
{
    depth += 2;
}

void NJS::Exdent()
{
    depth -= 2;
}

std::ostream& NJS::Spacing(std::ostream& os)
{
    for (unsigned i = 0; i < depth; ++i)
        os << ' ';
    return os;
}

std::string NJS::TypeString(const TypePtr& type)
{
    return type->GetString();
}
