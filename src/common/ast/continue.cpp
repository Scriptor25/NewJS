#include <newjs/ast.hpp>
#include <newjs/builder.hpp>

NJS::ContinueStatement::ContinueStatement(SourceLocation where)
    : Statement(std::move(where))
{
}

void NJS::ContinueStatement::PGenLLVM(Builder &builder) const
{
    const auto dest = builder.CurrentHeadBlock();
    if (!dest)
        Error(Where, "no control flow block to continue from");
    builder.GetBuilder().CreateBr(dest);
}

std::ostream &NJS::ContinueStatement::Print(std::ostream &stream) const
{
    return stream << "continue";
}
