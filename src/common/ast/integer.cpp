#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

NJS::IntegerExpression::IntegerExpression(SourceLocation where, TypePtr type, const uint64_t value)
    : Expression(std::move(where)),
      Type(std::move(type)),
      Value(value)
{
}

std::ostream &NJS::IntegerExpression::Print(std::ostream &stream) const
{
    if (Type)
        return Type->Print(stream << Value << ':');
    return stream << '(' << Value << ')';
}

NJS::ValuePtr NJS::IntegerExpression::_GenIntermediate(Builder &builder, const TypePtr &expected_type)
{
    const auto type = Type
                          ? Type
                          : expected_type && expected_type->IsInteger()
                                ? expected_type
                                : builder.GetTypeContext().GetIntegerType(64, true);

    return RValue::Create(
        builder,
        type,
        llvm::ConstantInt::get(type->GetLLVM(builder), Value, Type::As<IntegerType>(type)->IsSigned()));
}
