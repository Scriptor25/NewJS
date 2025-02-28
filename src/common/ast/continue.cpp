#include <newjs/ast.hpp>
#include <newjs/builder.hpp>

NJS::ContinueStatement::ContinueStatement(SourceLocation where)
    : Statement(std::move(where))
{
}

void NJS::ContinueStatement::GenLLVM(Builder &builder) const
{
    builder.GetBuilder().CreateBr(builder.CurrentHeadBlock());
}

std::ostream &NJS::ContinueStatement::Print(std::ostream &stream)
{
    return stream << "continue";
}
