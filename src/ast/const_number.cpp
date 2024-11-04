#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Value.hpp>

NJS::ConstNumberExpr::ConstNumberExpr(TypePtr type, const double value)
    : Expr(std::move(type)), Value(value)
{
}

NJS::ValuePtr NJS::ConstNumberExpr::GenLLVM(Builder& builder)
{
    const auto value = llvm::ConstantFP::get(builder.LLVMBuilder().getDoubleTy(), Value);
    return RValue::Create(builder, Type, value);
}

std::ostream& NJS::ConstNumberExpr::Print(std::ostream& os)
{
    return os << Value;
}
