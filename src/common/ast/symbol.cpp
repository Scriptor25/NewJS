#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>

NJS::SymbolExpression::SymbolExpression(SourceLocation where, std::string name)
    : Expression(std::move(where)),
      Name(std::move(name))
{
}

NJS::ValuePtr NJS::SymbolExpression::PGenLLVM(Builder &builder, const TypePtr &) const
{
    return builder.GetVariable(Name);
}

std::ostream &NJS::SymbolExpression::Print(std::ostream &stream)
{
    return stream << Name;
}
