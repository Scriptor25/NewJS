#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::BoolExpr::BoolExpr(SourceLocation where, const bool value)
    : Expr(std::move(where)), Value(value)
{
}

NJS::ValuePtr NJS::BoolExpr::GenLLVM(Builder& builder, const TypePtr&) const
{
    const auto result_type = builder.GetCtx().GetBoolType();
    const auto result_value = builder.GetBuilder().getInt1(Value);
    return RValue::Create(builder, result_type, result_value);
}

std::ostream& NJS::BoolExpr::Print(std::ostream& os)
{
    return os << (Value ? "true" : "false");
}
