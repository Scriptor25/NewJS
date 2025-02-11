#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>

NJS::SymbolExpr::SymbolExpr(SourceLocation where, std::string_view name)
    : Expr(std::move(where)),
      Name(std::move(name))
{
}

NJS::ValuePtr NJS::SymbolExpr::GenLLVM(Builder &builder, const TypePtr &) const
{
    return builder.GetVariable(Where, Name);
}

std::ostream &NJS::SymbolExpr::Print(std::ostream &os)
{
    return os << Name;
}
