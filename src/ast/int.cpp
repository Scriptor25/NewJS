#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Value.hpp>

#include "NJS/Type.hpp"

NJS::IntExpr::IntExpr(SourceLocation where, TypePtr type, const uint64_t value)
    : Expr(std::move(where), std::move(type)), Value(value)
{
}

NJS::ValuePtr NJS::IntExpr::GenLLVM(Builder& builder)
{
    const auto value = llvm::ConstantInt::get(Type->GetLLVM(builder), Value, Type->IsSigned());
    return RValue::Create(builder, Type, value);
}

std::ostream& NJS::IntExpr::Print(std::ostream& os)
{
    return os << Value;
}
