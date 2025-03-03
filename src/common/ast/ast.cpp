#include <utility>
#include <newjs/ast.hpp>

NJS::Statement::Statement(SourceLocation where)
    : Where(std::move(where))
{
}

void NJS::Statement::GenLLVM(Builder &builder) const try
{
    PGenLLVM(builder);
}
catch (const RTError &error)
{
    Error(error, Where, {});
}

NJS::Expression::Expression(SourceLocation where)
    : Statement(std::move(where))
{
}

NJS::ValuePtr NJS::Expression::GenLLVM(Builder &builder, const TypePtr &expected_type) const try
{
    return PGenLLVM(builder, expected_type);
}
catch (const RTError &error)
{
    Error(error, Where, {});
}

void NJS::Expression::PGenLLVM(Builder &builder) const try
{
    (void) GenLLVM(builder, {});
}
catch (const RTError &error)
{
    Error(error, Where, {});
}
