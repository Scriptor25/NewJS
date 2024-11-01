#pragma once

#include <map>
#include <memory>
#include <vector>

#include "Type.hpp"

namespace NJS
{
    typedef std::shared_ptr<struct Param> ParamPtr;

    typedef std::shared_ptr<struct Stmt> StmtPtr;
    typedef std::shared_ptr<struct Expr> ExprPtr;

    typedef std::shared_ptr<struct ScopeStmt> ScopeStmtPtr;
    typedef std::shared_ptr<struct FunctionStmt> FunctionStmtPtr;
    typedef std::shared_ptr<struct VariableStmt> VariableStmtPtr;
    typedef std::shared_ptr<struct IfStmt> IfStmtPtr;
    typedef std::shared_ptr<struct ForStmt> ForStmtPtr;
    typedef std::shared_ptr<struct ForInOfStmt> ForInOfStmtPtr;
    typedef std::shared_ptr<struct ReturnStmt> ReturnStmtPtr;

    std::ostream& operator<<(std::ostream&, const StmtPtr&);
    std::ostream& operator<<(std::ostream&, const ExprPtr&);

    struct Stmt
    {
        virtual ~Stmt() = default;
        virtual std::ostream& Print(std::ostream&) = 0;
    };

    struct ScopeStmt : Stmt
    {
        explicit ScopeStmt(std::vector<StmtPtr> children);

        std::ostream& Print(std::ostream&) override;

        std::vector<StmtPtr> Children;
    };

    struct FunctionStmt : Stmt
    {
        FunctionStmt(std::string name, std::vector<ParamPtr> params, TypePtr, ScopeStmt body);

        std::ostream& Print(std::ostream&) override;

        std::string Name;
        std::vector<ParamPtr> Params;
        TypePtr ResultType;
        ScopeStmt Body;
    };

    struct VariableStmt : Stmt
    {
        VariableStmt(bool is_const, ParamPtr name, ExprPtr value);

        std::ostream& Print(std::ostream&) override;

        bool IsConst;
        ParamPtr Name;
        ExprPtr Value;
    };

    struct IfStmt : Stmt
    {
        IfStmt(ExprPtr condition, StmtPtr then, StmtPtr else_);

        std::ostream& Print(std::ostream&) override;

        ExprPtr Condition;
        StmtPtr Then;
        StmtPtr Else;
    };

    struct ForStmt : Stmt
    {
        ForStmt(StmtPtr init, ExprPtr condition, StmtPtr loop, StmtPtr body);

        std::ostream& Print(std::ostream&) override;

        StmtPtr Init;
        ExprPtr Condition;
        StmtPtr Loop;
        StmtPtr Body;
    };

    struct ForInOfStmt : Stmt
    {
        ForInOfStmt(StmtPtr, bool of, ExprPtr value, StmtPtr body);

        std::ostream& Print(std::ostream&) override;

        StmtPtr Init;
        bool Of;
        ExprPtr Value;
        StmtPtr Body;
    };

    struct ReturnStmt : Stmt
    {
        explicit ReturnStmt(ExprPtr value);

        std::ostream& Print(std::ostream&) override;

        ExprPtr Value;
    };

    struct Expr : Stmt
    {
        explicit Expr(TypePtr);

        TypePtr Type;
    };

    struct ArrayExpr : Expr
    {
        ArrayExpr(TypePtr, std::vector<ExprPtr>);

        std::ostream& Print(std::ostream&) override;

        std::vector<ExprPtr> Entries;
    };

    struct BinaryExpr : Expr
    {
        BinaryExpr(TypePtr, std::string, ExprPtr, ExprPtr);

        std::ostream& Print(std::ostream&) override;

        std::string Op;
        ExprPtr LHS;
        ExprPtr RHS;
    };

    struct CallExpr : Expr
    {
        CallExpr(TypePtr, ExprPtr, std::vector<ExprPtr>);

        std::ostream& Print(std::ostream&) override;

        ExprPtr Callee;
        std::vector<ExprPtr> Args;
    };

    struct FormatExpr : Expr
    {
        FormatExpr(TypePtr, size_t, std::map<size_t, std::string>, std::map<size_t, ExprPtr>);

        std::ostream& Print(std::ostream&) override;

        size_t Count;
        std::map<size_t, std::string> Statics;
        std::map<size_t, ExprPtr> Dynamics;
    };

    struct FunctionExpr : Expr
    {
        FunctionExpr(TypePtr, std::vector<ParamPtr>, ScopeStmt);

        std::ostream& Print(std::ostream&) override;

        std::vector<ParamPtr> Params;
        ScopeStmt Body;
    };

    struct MemberExpr : Expr
    {
        MemberExpr(TypePtr, ExprPtr, std::string);

        std::ostream& Print(std::ostream&) override;

        ExprPtr Object;
        std::string Member;
    };

    struct NumberExpr : Expr
    {
        NumberExpr(TypePtr, double);

        std::ostream& Print(std::ostream&) override;

        double Value;
    };

    struct ObjectExpr : Expr
    {
        ObjectExpr(TypePtr, std::map<std::string, ExprPtr>);

        std::ostream& Print(std::ostream&) override;

        std::map<std::string, ExprPtr> Entries;
    };

    struct StringExpr : Expr
    {
        StringExpr(TypePtr, std::string);

        std::ostream& Print(std::ostream&) override;

        std::string Value;
    };

    struct SubscriptExpr : Expr
    {
        SubscriptExpr(TypePtr, ExprPtr, ExprPtr);

        std::ostream& Print(std::ostream&) override;

        ExprPtr Array;
        ExprPtr Index;
    };

    struct SymbolExpr : Expr
    {
        SymbolExpr(TypePtr, std::string);

        std::ostream& Print(std::ostream&) override;

        std::string Name;
    };

    struct UnaryExpr : Expr
    {
        UnaryExpr(TypePtr, std::string, bool, ExprPtr);

        std::ostream& Print(std::ostream&) override;

        std::string Op;
        bool OpRight;
        ExprPtr Operand;
    };
}
