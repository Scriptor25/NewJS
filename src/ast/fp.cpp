#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::FPExpr::FPExpr(SourceLocation where, TypePtr type, const double value)
    : Expr(std::move(where), std::move(type)), Value(value)
{
}

NJS::ValuePtr NJS::FPExpr::GenLLVM(Builder& builder)
{
    const auto value = llvm::ConstantFP::get(Type->GetLLVM(builder), Value);
    return RValue::Create(builder, Type, value);
}

std::ostream& NJS::FPExpr::Print(std::ostream& os)
{
    return os << Value;
}
