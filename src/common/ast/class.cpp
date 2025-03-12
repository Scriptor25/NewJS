#include <newjs/ast.hpp>

NJS::ClassStatement::ClassStatement(SourceLocation where, std::string name)
    : Statement(std::move(where)),
      Name(std::move(name))
{
}

std::ostream &NJS::ClassStatement::Print(std::ostream &stream) const
{
    return stream << "class " << Name;
}

void NJS::ClassStatement::PGenLLVM(Builder &builder) const
{
}
