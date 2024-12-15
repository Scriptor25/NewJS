#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::SymbolExpr::SymbolExpr(SourceLocation where, TypePtr type, std::string name)
    : Expr(std::move(where), std::move(type)), Name(std::move(name))
{
}

NJS::ValuePtr NJS::SymbolExpr::GenLLVM(Builder& builder)
{
    if (Name == "_")
        return RValue::Create(builder, builder.GetCtx().GetNoType(), nullptr);
    return builder.GetVar(Name);
}

std::ostream& NJS::SymbolExpr::Print(std::ostream& os)
{
    return os << Name;
}
