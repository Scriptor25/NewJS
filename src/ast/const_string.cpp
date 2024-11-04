#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Context.hpp>
#include <NJS/Value.hpp>

NJS::ConstStringExpr::ConstStringExpr(std::string value)
    : Value(std::move(value))
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
