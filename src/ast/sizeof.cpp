#include <utility>
#include <llvm/IR/Constants.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::SizeOfExpression::SizeOfExpression(SourceLocation where, ExpressionPtr operand)
    : Expression(std::move(where)),
      Operand(std::move(operand))
{
}

NJS::ValuePtr NJS::SizeOfExpression::GenLLVM(Builder &builder, const TypePtr &expected_type) const
{
    const auto operand = Operand->GenLLVM(builder, {});

    const auto type = expected_type && expected_type->IsInteger()
                          ? expected_type
                          : builder.GetTypeContext().GetIntegerType(64, false);
    const auto value = llvm::ConstantInt::get(
        type->GetLLVM(Where, builder),
        operand->GetType()->GetSize(),
        type->IsSigned(Where));
    return RValue::Create(builder, type, value);
}

std::ostream &NJS::SizeOfExpression::Print(std::ostream &stream)
{
    return Operand->Print(stream << "sizeof(") << ")";
}
