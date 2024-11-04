#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>

NJS::SymbolExpr::SymbolExpr(std::string name)
    : Name(std::move(name))
{
}

NJS::ValuePtr NJS::SymbolExpr::GenLLVM(Builder& builder)
{
    return builder.GetVar(Name);
}

std::ostream& NJS::SymbolExpr::Print(std::ostream& os)
{
    return os << Name;
}
