#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::BoolExpr::BoolExpr(SourceLocation where, const bool value)
    : Expr(std::move(where)), Value(value)
{
}

NJS::ValuePtr NJS::BoolExpr::GenLLVM(Builder& builder, const TypePtr&)
{
    const auto type = builder.GetCtx().GetBoolType();
    const auto value = builder.GetBuilder().getInt1(Value);
    return RValue::Create(builder, type, value);
}

std::ostream& NJS::BoolExpr::Print(std::ostream& os)
{
    return os << (Value ? "true" : "false");
}
