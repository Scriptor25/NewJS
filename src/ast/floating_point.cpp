#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::FloatingPointExpression::FloatingPointExpression(SourceLocation where, TypePtr type, const double value)
    : Expression(std::move(where)),
      Type(std::move(type)),
      Value(value)
{
}

NJS::ValuePtr NJS::FloatingPointExpression::GenLLVM(Builder &builder, const TypePtr &expected_type) const
{
    const auto result_type = Type
                                 ? Type
                                 : expected_type && expected_type->IsFloatingPoint()
                                       ? expected_type
                                       : builder.GetTypeContext().GetFloatingPointType(64);
    const auto result_value = llvm::ConstantFP::get(result_type->GetLLVM(Where, builder), Value);
    return RValue::Create(builder, result_type, result_value);
}

std::ostream &NJS::FloatingPointExpression::Print(std::ostream &stream)
{
    return stream << Value;
}
