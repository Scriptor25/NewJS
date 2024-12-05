#pragma once

#include <memory>

namespace NJS
{
    class Context;
    class Builder;

    struct SourceLocation;
    struct Token;

    typedef std::shared_ptr<struct Type> TypePtr;
    typedef std::shared_ptr<struct Param> ParamPtr;
    typedef std::shared_ptr<class Value> ValuePtr;

    typedef std::shared_ptr<struct Stmt> StmtPtr;
    typedef std::shared_ptr<struct Expr> ExprPtr;
    typedef std::shared_ptr<struct ImportStmt> ImportStmtPtr;
    typedef std::shared_ptr<struct ScopeStmt> ScopeStmtPtr;
    typedef std::shared_ptr<struct FunctionStmt> FunctionStmtPtr;
    typedef std::shared_ptr<struct VariableStmt> VariableStmtPtr;
    typedef std::shared_ptr<struct IfStmt> IfStmtPtr;
    typedef std::shared_ptr<struct ForStmt> ForStmtPtr;
    typedef std::shared_ptr<struct ForInOfStmt> ForInOfStmtPtr;
    typedef std::shared_ptr<struct ReturnStmt> ReturnStmtPtr;

    void Indent();
    void Exdent();
    std::ostream& Spacing(std::ostream&);
}
