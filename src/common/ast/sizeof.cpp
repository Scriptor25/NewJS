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

std::ostream &NJS::SizeOfExpression::Print(std::ostream &stream) const
{
    return Operand->Print(stream << "sizeof(") << ")";
}

NJS::ValuePtr NJS::SizeOfExpression::_GenIntermediate(Builder &builder, const TypePtr &expected_type)
{
    const auto operand = Operand->GenIntermediate(builder, nullptr);

    const auto type = expected_type && expected_type->IsInteger()
                          ? expected_type
                          : builder.GetTypeContext().GetIntegerType(64, false);

    const auto size_value = llvm::ConstantInt::get(
        type->GetLLVM(builder),
        operand->GetType()->GetSize(builder),
        Type::As<IntegerType>(type)->IsSigned());
    return RValue::Create(builder, type, size_value);
}
