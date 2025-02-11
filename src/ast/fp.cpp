#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::FPExpr::FPExpr(SourceLocation where, TypePtr type, const double value)
    : Expr(std::move(where)),
      Type(std::move(type)),
      Value(value)
{
}

NJS::ValuePtr NJS::FPExpr::GenLLVM(Builder &builder, const TypePtr &expected) const
{
    const auto result_type = Type
                                 ? Type
                                 : expected && expected->IsFP()
                                       ? expected
                                       : builder.GetCtx().GetFPType(64);
    const auto result_value = llvm::ConstantFP::get(result_type->GetLLVM(Where, builder), Value);
    return RValue::Create(builder, result_type, result_value);
}

std::ostream &NJS::FPExpr::Print(std::ostream &os)
{
    return os << Value;
}
