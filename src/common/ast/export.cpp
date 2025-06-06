#include <newjs/ast.hpp>

NJS::ExportStatement::ExportStatement(SourceLocation where, StatementPtr value)
    : Statement(std::move(where)),
      Value(std::move(value))
{
}

std::ostream &NJS::ExportStatement::Print(std::ostream &stream) const
{
    return Value->Print(stream << "export ");
}

void NJS::ExportStatement::_GenIntermediate(Builder &builder, bool)
{
    Value->GenIntermediate(builder, true);
}
