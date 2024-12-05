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

    os << '{' << std::endl;
    Indent();
    for (const auto& child : Children)
        child->Print(Spacing(os)) << std::endl;
    Exdent();
    return Spacing(os) << '}';
}

NJS::ScopeExpr::ScopeExpr(std::vector<StmtPtr> children, ExprPtr last)
    : Children(std::move(children)), Last(std::move(last))
{
}

NJS::ValuePtr NJS::ScopeExpr::GenLLVM(Builder& builder)
{
    builder.Push();
    for (const auto& child : Children)
        child->GenLLVM(builder);
    auto result = Last->GenLLVM(builder);
    builder.Pop();
    return result;
}

std::ostream& NJS::ScopeExpr::Print(std::ostream& os)
{
    if (Children.empty()) return Last->Print(os << "{ ") << " }";

    os << '{' << std::endl;
    Indent();
    for (const auto& child : Children)
        child->Print(Spacing(os)) << std::endl;
    Last->Print(Spacing(os)) << std::endl;
    Exdent();
    return Spacing(os) << '}';
}
