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

NJS::ValuePtr NJS::IntExpr::GenLLVM(Builder& builder, const TypePtr& expected) const
{
    const auto result_type = Type
                                 ? Type
                                 : expected && expected->IsInt()
                                 ? expected
                                 : builder.GetCtx().GetIntType(64, true);
    const auto result_value = llvm::ConstantInt::get(result_type->GetLLVM(Where, builder), Value,
                                                     result_type->IsSigned());
    return RValue::Create(builder, result_type, result_value);
}

std::ostream& NJS::IntExpr::Print(std::ostream& os)
{
    return os << Value;
}
