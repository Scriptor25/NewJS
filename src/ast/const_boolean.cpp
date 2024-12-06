#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Context.hpp>
#include <NJS/Value.hpp>

NJS::ConstBooleanExpr::ConstBooleanExpr(const bool value)
    : Value(value)
{
}

NJS::ValuePtr NJS::ConstBooleanExpr::GenLLVM(Builder& builder)
{
    const auto type = builder.GetCtx().GetBooleanType();
    const auto value = builder.GetBuilder().getInt1(Value);
    return RValue::Create(builder, type, value);
}

std::ostream& NJS::ConstBooleanExpr::Print(std::ostream& os)
{
    return os << (Value ? "true" : "false");
}
