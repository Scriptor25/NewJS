#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/NJS.hpp>

NJS::ScopeStmt::ScopeStmt(std::vector<StmtPtr> children)
    : Children(std::move(children))
{
}

NJS::ValuePtr NJS::ScopeStmt::GenLLVM(Builder& builder)
{
    builder.Push();
    for (const auto& child : Children)
        child->GenLLVM(builder);
    builder.Pop();
    return {};
}

std::ostream& NJS::ScopeStmt::Print(std::ostream& os)
{
    if (Children.empty()) return os << "{}";

    os << "{" << std::endl;
    Indent();
    for (const auto& child : Children)
        Spacing(os) << child << std::endl;
    Exdent();
    return Spacing(os) << "}";
}
