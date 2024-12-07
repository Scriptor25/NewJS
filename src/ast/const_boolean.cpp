#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Value.hpp>

NJS::ConstBooleanExpr::ConstBooleanExpr(SourceLocation where, TypePtr type, const bool value)
    : Expr(std::move(where), std::move(type)), Value(value)
{
}

NJS::ValuePtr NJS::ConstBooleanExpr::GenLLVM(Builder& builder)
{
    const auto value = builder.GetBuilder().getInt1(Value);
    return RValue::Create(builder, Type, value);
}

std::ostream& NJS::ConstBooleanExpr::Print(std::ostream& os)
{
    return os << (Value ? "true" : "false");
}
