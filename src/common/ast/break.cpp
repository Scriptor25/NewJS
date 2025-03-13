#include <newjs/ast.hpp>
#include <newjs/builder.hpp>

NJS::BreakStatement::BreakStatement(SourceLocation where)
    : Statement(std::move(where))
{
}

void NJS::BreakStatement::PGenLLVM(Builder &builder)
{
    const auto dest = builder.CurrentTailBlock();
    if (!dest)
        Error(Where, "no control flow block to break to");
    builder.GetBuilder().CreateBr(dest);
}

std::ostream &NJS::BreakStatement::Print(std::ostream &stream) const
{
    return stream << "break";
}
