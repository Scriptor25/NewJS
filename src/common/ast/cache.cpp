#include <newjs/ast.hpp>

NJS::CacheExpression::CacheExpression(SourceLocation where, ExpressionPtr value)
    : Expression(std::move(where)),
      Value(std::move(value))
{
}

std::ostream &NJS::CacheExpression::Print(std::ostream &stream) const
{
    return Value->Print(stream << "cache ");
}

NJS::ValuePtr NJS::CacheExpression::PGenLLVM(Builder &builder, const TypePtr &expected_type)
{
    if (Cache)
        return Cache;
    return Cache = Value->GenLLVM(builder, expected_type);
}
