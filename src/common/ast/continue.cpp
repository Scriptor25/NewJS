#include <newjs/ast.hpp>
#include <newjs/builder.hpp>

NJS::ContinueStatement::ContinueStatement(SourceLocation where)
    : Statement(std::move(where))
{
}

std::ostream &NJS::ContinueStatement::Print(std::ostream &stream) const
{
    return stream << "continue";
}

void NJS::ContinueStatement::_GenIntermediate(Builder &builder, bool)
{
    const auto dest = builder.CurrentHeadBlock();
    if (!dest)
        Error(Where, "no control flow block to continue from");
    builder.GetBuilder().CreateBr(dest);
}
