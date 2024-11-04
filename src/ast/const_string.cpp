#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Value.hpp>

NJS::ConstStringExpr::ConstStringExpr(TypePtr type, std::string value)
    : Expr(std::move(type)), Value(std::move(value))
{
}

NJS::ValuePtr NJS::ConstStringExpr::GenLLVM(Builder& builder)
{
    const auto ptr = builder.LLVMBuilder().CreateGlobalStringPtr(Value);
    return LValue::Create(builder, builder.Ctx().GetStringType(), ptr);
}

std::ostream& NJS::ConstStringExpr::Print(std::ostream& os)
{
    return os << '"' << Value << '"';
}
