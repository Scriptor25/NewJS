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
catch (RTError &error)
{
    Error(Where, std::move(error));
}

NJS::Expression::Expression(SourceLocation where)
    : Statement(std::move(where))
{
}

NJS::ValuePtr NJS::Expression::GenLLVM(Builder &builder, const TypePtr &expected_type) const try
{
    return PGenLLVM(builder, expected_type);
}
catch (RTError &error)
{
    Error(Where, std::move(error));
}

void NJS::Expression::PGenLLVM(Builder &builder) const try
{
    (void) GenLLVM(builder, {});
}
catch (RTError &error)
{
    Error(Where, std::move(error));
}
