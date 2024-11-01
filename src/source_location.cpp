#include <NJS/SourceLocation.hpp>

std::ostream& NJS::operator<<(std::ostream& os, const SourceLocation& ref)
{
    return os << ref.Filename << ':' << ref.Row << ':' << ref.Col;
}
