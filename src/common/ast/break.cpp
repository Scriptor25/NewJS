#include <newjs/ast.hpp>
#include <newjs/builder.hpp>

NJS::BreakStatement::BreakStatement(SourceLocation where)
    : Statement(std::move(where))
{
}

std::ostream &NJS::BreakStatement::Print(std::ostream &stream) const
{
    return stream << "break";
}

void NJS::BreakStatement::_GenIntermediate(Builder &builder, bool)
{
    const auto dest = builder.CurrentTailBlock();
    if (!dest)
        Error(Where, "no control flow block to break to");
    builder.GetBuilder().CreateBr(dest);
}
