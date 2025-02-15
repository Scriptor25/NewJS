#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::IntegerExpression::IntegerExpression(SourceLocation where, TypePtr type, const uint64_t value)
    : Expression(std::move(where)),
      Type(std::move(type)),
      Value(value)
{
}

NJS::ValuePtr NJS::IntegerExpression::GenLLVM(Builder &builder, const TypePtr &expected_type) const
{
    const auto result_type = Type
                                 ? Type
                                 : expected_type && expected_type->IsInteger()
                                       ? expected_type
                                       : builder.GetTypeContext().GetIntegerType(64, true);
    const auto result_value = llvm::ConstantInt::get(
        result_type->GetLLVM(Where, builder),
        Value,
        result_type->IsSigned());
    return RValue::Create(builder, result_type, result_value);
}

std::ostream &NJS::IntegerExpression::Print(std::ostream &stream)
{
    return stream << Value;
}
