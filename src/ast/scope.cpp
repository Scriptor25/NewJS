#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>

NJS::ScopeStmt::ScopeStmt(SourceLocation where, std::vector<StmtPtr> children)
    : Stmt(std::move(where)), Children(std::move(children))
{
}

void NJS::ScopeStmt::GenVoidLLVM(Builder& builder)
{
    builder.Push();
    for (const auto& child : Children)
        child->GenVoidLLVM(builder);
    builder.Pop();
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

NJS::ScopeExpr::ScopeExpr(SourceLocation where, std::vector<StmtPtr> children, ExprPtr last)
    : Expr(std::move(where)), Children(std::move(children)), Last(std::move(last))
{
}

NJS::ValuePtr NJS::ScopeExpr::GenLLVM(Builder& builder, const TypePtr& expected)
{
    builder.Push();
    for (const auto& child : Children)
        child->GenVoidLLVM(builder);
    auto result = Last->GenLLVM(builder, expected);
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
