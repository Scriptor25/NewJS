#include <newjs/ast.hpp>
#include <newjs/builder.hpp>

NJS::BreakStatement::BreakStatement(SourceLocation where)
    : Statement(std::move(where))
{
}

bool NJS::BreakStatement::GenLLVM(Builder &builder) const
{
    const auto dest = builder.CurrentTailBlock();
    if (!dest)
        return true;
    builder.GetBuilder().CreateBr(dest);
    return false;
}

std::ostream &NJS::BreakStatement::Print(std::ostream &stream)
{
    return stream << "break";
}
