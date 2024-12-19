#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Value.hpp>

NJS::CharExpr::CharExpr(SourceLocation where, TypePtr type, const char value)
    : Expr(std::move(where), std::move(type)), Value(value)
{
}

NJS::ValuePtr NJS::CharExpr::GenLLVM(Builder& builder)
{
    const auto value = builder.GetBuilder().getInt8(Value);
    return RValue::Create(builder, Type, value);
}

std::ostream& NJS::CharExpr::Print(std::ostream& os)
{
    return os << '\'' << Value << '\'';
}
