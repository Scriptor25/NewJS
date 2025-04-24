#include <newjs/ast.hpp>

NJS::ClassStatement::ClassStatement(SourceLocation where, std::string name)
    : Statement(std::move(where)),
      Name(std::move(name))
{
}

NJS::ClassStatement::ClassStatement(SourceLocation where, std::string name, std::vector<ExpressionPtr> functions)
    : Statement(std::move(where)),
      Name(std::move(name)),
      Functions(std::move(functions))
{
}

std::ostream &NJS::ClassStatement::Print(std::ostream &stream) const
{
    return stream << "class " << Name;
}

void NJS::ClassStatement::_GenIntermediate(Builder &builder, bool is_export)
{
    for (const auto &function: Functions)
        (void) function->GenIntermediate(builder, nullptr);
}
