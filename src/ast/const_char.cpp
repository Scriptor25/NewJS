#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Context.hpp>
#include <NJS/Value.hpp>

NJS::ConstCharExpr::ConstCharExpr(const char value)
    : Value(value)
{
}

NJS::ValuePtr NJS::ConstCharExpr::GenLLVM(Builder& builder)
{
    const auto type = builder.GetCtx().GetCharType();
    const auto value = builder.GetBuilder().getInt8(Value);
    return RValue::Create(builder, type, value);
}

std::ostream& NJS::ConstCharExpr::Print(std::ostream& os)
{
    return os << '\'' << Value << '\'';
}
