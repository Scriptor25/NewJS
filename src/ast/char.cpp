#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::CharExpr::CharExpr(SourceLocation where, const char value)
    : Expr(std::move(where)), Value(value)
{
}

NJS::ValuePtr NJS::CharExpr::GenLLVM(Builder& builder)
{
    const auto type = builder.GetCtx().GetCharType();
    const auto value = builder.GetBuilder().getInt8(Value);
    return RValue::Create(builder, type, value);
}

std::ostream& NJS::CharExpr::Print(std::ostream& os)
{
    return os << '\'' << Value << '\'';
}
