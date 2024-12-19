#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Value.hpp>

NJS::BoolExpr::BoolExpr(SourceLocation where, TypePtr type, const bool value)
    : Expr(std::move(where), std::move(type)), Value(value)
{
}

NJS::ValuePtr NJS::BoolExpr::GenLLVM(Builder& builder)
{
    const auto value = builder.GetBuilder().getInt1(Value);
    return RValue::Create(builder, Type, value);
}

std::ostream& NJS::BoolExpr::Print(std::ostream& os)
{
    return os << (Value ? "true" : "false");
}
