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

void NJS::Expression::GenLLVM(Builder &builder) const
{
    (void) GenLLVM(builder, {});
}
