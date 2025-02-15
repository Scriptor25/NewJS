#include <utility>
#include <NJS/AST.hpp>

NJS::Statement::Statement(SourceLocation where)
    : Where(std::move(where))
{
}

NJS::Expression::Expression(SourceLocation where)
    : Statement(std::move(where))
{
}

void NJS::Expression::GenVoidLLVM(Builder &builder) const
{
    (void) GenLLVM(builder, {});
}
