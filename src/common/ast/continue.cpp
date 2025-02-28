#include <newjs/ast.hpp>
#include <newjs/builder.hpp>

NJS::ContinueStatement::ContinueStatement(SourceLocation where)
    : Statement(std::move(where))
{
}

bool NJS::ContinueStatement::GenLLVM(Builder &builder) const
{
    const auto dest = builder.CurrentHeadBlock();
    if (!dest)
        return true;
    builder.GetBuilder().CreateBr(dest);
    return false;
}

std::ostream &NJS::ContinueStatement::Print(std::ostream &stream)
{
    return stream << "continue";
}
