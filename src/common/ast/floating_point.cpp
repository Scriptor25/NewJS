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

std::ostream &NJS::FloatingPointExpression::Print(std::ostream &stream) const
{
    return stream << Value;
}

NJS::ValuePtr NJS::FloatingPointExpression::PGenLLVM(Builder &builder, const TypePtr &expected_type)
{
    const auto type = Type
                          ? Type
                          : expected_type && expected_type->IsFloatingPoint()
                                ? expected_type
                                : builder.GetTypeContext().GetFloatingPointType(64);

    return RValue::Create(builder, type, llvm::ConstantFP::get(type->GetLLVM(builder), Value));
}
