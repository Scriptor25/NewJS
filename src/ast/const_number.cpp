#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Value.hpp>

NJS::ConstNumberExpr::ConstNumberExpr(SourceLocation where, TypePtr type, const double value)
    : Expr(std::move(where), std::move(type)), Value(value)
{
}

NJS::ValuePtr NJS::ConstNumberExpr::GenLLVM(Builder& builder)
{
    const auto value = llvm::ConstantFP::get(builder.GetBuilder().getDoubleTy(), Value);
    return RValue::Create(builder, Type, value);
}

std::ostream& NJS::ConstNumberExpr::Print(std::ostream& os)
{
    return os << Value;
}
