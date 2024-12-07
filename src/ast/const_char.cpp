#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Value.hpp>

NJS::ConstCharExpr::ConstCharExpr(SourceLocation where, TypePtr type, const char value)
    : Expr(std::move(where), std::move(type)), Value(value)
{
}

NJS::ValuePtr NJS::ConstCharExpr::GenLLVM(Builder& builder)
{
    const auto value = builder.GetBuilder().getInt8(Value);
    return RValue::Create(builder, Type, value);
}

std::ostream& NJS::ConstCharExpr::Print(std::ostream& os)
{
    return os << '\'' << Value << '\'';
}
