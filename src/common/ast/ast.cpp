#include <utility>
#include <newjs/ast.hpp>

NJS::Statement::Statement(SourceLocation where)
    : Where(std::move(where))
{
}

NJS::Expression::Expression(SourceLocation where)
    : Statement(std::move(where))
{
}

NJS::ValuePtr NJS::Expression::GenLLVM(Builder &builder) const
{
    return GenLLVM(builder, {});
}
