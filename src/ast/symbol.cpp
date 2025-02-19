#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>

NJS::SymbolExpression::SymbolExpression(SourceLocation where, std::string name)
    : Expression(std::move(where)),
      Name(std::move(name))
{
}

NJS::ValuePtr NJS::SymbolExpression::GenLLVM(Builder &builder, const TypePtr &) const
{
    return builder.GetVariable(Where, Name);
}

std::ostream &NJS::SymbolExpression::Print(std::ostream &stream)
{
    return stream << Name;
}
