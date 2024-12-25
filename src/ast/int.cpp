#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::IntExpr::IntExpr(SourceLocation where, TypePtr type, const uint64_t value)
    : Expr(std::move(where)), Type(std::move(type)), Value(value)
{
}

NJS::ValuePtr NJS::IntExpr::GenLLVM(Builder& builder, const TypePtr& expected)
{
    const auto type = Type
                          ? Type
                          : expected && expected->IsInt()
                          ? expected
                          : builder.GetCtx().GetIntType(64, true);
    const auto value = llvm::ConstantInt::get(type->GetLLVM(builder), Value, type->IsSigned());
    return RValue::Create(builder, type, value);
}

std::ostream& NJS::IntExpr::Print(std::ostream& os)
{
    return os << Value;
}
