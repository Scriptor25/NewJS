#include <newjs/ast.hpp>
#include <newjs/builder.hpp>

NJS::BreakStatement::BreakStatement(SourceLocation where)
    : Statement(std::move(where))
{
}

void NJS::BreakStatement::GenLLVM(Builder &builder) const
{
    builder.GetBuilder().CreateBr(builder.CurrentTailBlock());
}

std::ostream &NJS::BreakStatement::Print(std::ostream &stream)
{
    return stream << "break";
}
