#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>

NJS::SymbolExpression::SymbolExpression(SourceLocation where, std::string name)
    : Expression(std::move(where)),
      Name(std::move(name))
{
}

std::ostream &NJS::SymbolExpression::Print(std::ostream &stream) const
{
    return stream << Name;
}

NJS::ValuePtr NJS::SymbolExpression::PGenLLVM(Builder &builder, const TypePtr &)
{
    return builder.GetVariable(Name);
}
