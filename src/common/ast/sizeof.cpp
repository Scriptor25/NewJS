#include <utility>
#include <llvm/IR/Constants.h>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

NJS::SizeOfExpression::SizeOfExpression(SourceLocation where, ExpressionPtr operand)
    : Expression(std::move(where)),
      Operand(std::move(operand))
{
}

NJS::ValuePtr NJS::SizeOfExpression::GenLLVM(
    Builder &builder,
    const TypePtr &expected_type) const
{
    const auto value = Operand->GenLLVM(builder, {});
    const auto type = expected_type && expected_type->IsInteger()
                          ? expected_type
                          : builder.GetTypeContext().GetIntegerType(64, false);
    const auto size = llvm::ConstantInt::get(
        type->GetLLVM(Where, builder),
        value->GetType()->GetSize(Where, builder),
        type->IsSigned(Where));
    return RValue::Create(builder, type, size);
}

std::ostream &NJS::SizeOfExpression::Print(std::ostream &stream)
{
    return Operand->Print(stream << "sizeof(") << ")";
}
