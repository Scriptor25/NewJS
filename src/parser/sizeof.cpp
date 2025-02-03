#include <utility>
#include <llvm/IR/Constants.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::SizeOfExpr::SizeOfExpr(SourceLocation where, ExprPtr operand)
    : Expr(std::move(where)), Operand(std::move(operand))
{
}

NJS::ValuePtr NJS::SizeOfExpr::GenLLVM(Builder& builder, const TypePtr& expected) const
{
    const auto operand = Operand->GenLLVM(builder, {});

    const auto type = expected && expected->IsInt()
                          ? expected
                          : builder.GetCtx().GetIntType(64, false);
    const auto value = llvm::ConstantInt::get(
        type->GetLLVM(Where, builder),
        operand->GetType()->GetSize(),
        type->IsSigned());
    return RValue::Create(builder, type, value);
}

std::ostream& NJS::SizeOfExpr::Print(std::ostream& os)
{
    return Operand->Print(os << "sizeof(") << ")";
}
