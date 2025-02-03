#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::CharExpr::CharExpr(SourceLocation where, const char value)
    : Expr(std::move(where)), Value(value)
{
}

NJS::ValuePtr NJS::CharExpr::GenLLVM(Builder& builder, const TypePtr&) const
{
    const auto result_type = builder.GetCtx().GetCharType();
    const auto result_value = builder.GetBuilder().getInt8(Value);
    return RValue::Create(builder, result_type, result_value);
}

std::ostream& NJS::CharExpr::Print(std::ostream& os)
{
    return os << '\'' << Value << '\'';
}
