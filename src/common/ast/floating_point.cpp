#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

NJS::FloatingPointExpression::FloatingPointExpression(SourceLocation where, TypePtr type, const double value)
    : Expression(std::move(where)),
      Type(std::move(type)),
      Value(value)
{
}

NJS::ValuePtr NJS::FloatingPointExpression::GenLLVM(
    Builder &builder,
    ErrorInfo &error,
    const TypePtr &expected_type) const
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
