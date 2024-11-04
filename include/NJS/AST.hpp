#pragma once

#include <map>
#include <memory>
#include <vector>

namespace NJS
{
    class Builder;

    typedef std::shared_ptr<struct Type> TypePtr;
    typedef std::shared_ptr<struct Param> ParamPtr;
    typedef std::shared_ptr<class Value> ValuePtr;

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
        virtual ValuePtr GenLLVM(Builder&) = 0;
        virtual std::ostream& Print(std::ostream&) = 0;
    };

    struct ScopeStmt : Stmt
    {
        explicit ScopeStmt(std::vector<StmtPtr> children);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::vector<StmtPtr> Children;
    };

    struct FunctionStmt : Stmt
    {
        FunctionStmt(std::string name, std::vector<ParamPtr> params, TypePtr, ScopeStmtPtr body);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::string Name;
        std::vector<ParamPtr> Params;
        TypePtr Type;
        ScopeStmtPtr Body;
    };

    struct IfStmt : Stmt
    {
        IfStmt(ExprPtr condition, StmtPtr then, StmtPtr else_);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Condition;
        StmtPtr Then;
        StmtPtr Else;
    };

    struct ForStmt : Stmt
    {
        ForStmt(StmtPtr init, ExprPtr condition, StmtPtr loop, StmtPtr body);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        StmtPtr Init;
        ExprPtr Condition;
        StmtPtr Loop;
        StmtPtr Body;
    };

    struct ForInOfStmt : Stmt
    {
        ForInOfStmt(StmtPtr, bool of, ExprPtr value, StmtPtr body);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        StmtPtr Init;
        bool Of;
        ExprPtr Value;
        StmtPtr Body;
    };

    struct ReturnStmt : Stmt
    {
        explicit ReturnStmt(ExprPtr value);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Value;
    };

    struct VariableStmt : Stmt
    {
        VariableStmt(bool is_const, ParamPtr name, ExprPtr value);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        bool IsConst;
        ParamPtr Name;
        ExprPtr Value;
    };

    struct Expr : Stmt
    {
        explicit Expr(TypePtr);

        TypePtr Type;
    };

    struct BinaryExpr : Expr
    {
        BinaryExpr(TypePtr, std::string, ExprPtr, ExprPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::string Op;
        ExprPtr LHS;
        ExprPtr RHS;
    };

    struct CallExpr : Expr
    {
        CallExpr(TypePtr, ExprPtr, std::vector<ExprPtr>);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Callee;
        std::vector<ExprPtr> Args;
    };

    struct ConstTupleExpr : Expr
    {
        ConstTupleExpr(TypePtr, std::vector<ExprPtr>);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::vector<ExprPtr> Entries;
    };

    struct ConstFunctionExpr : Expr
    {
        ConstFunctionExpr(TypePtr, std::vector<ParamPtr>, ScopeStmt);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::vector<ParamPtr> Params;
        ScopeStmt Body;
    };

    struct ConstNumberExpr : Expr
    {
        ConstNumberExpr(TypePtr, double);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        double Value;
    };

    struct ConstObjectExpr : Expr
    {
        ConstObjectExpr(TypePtr, std::map<std::string, ExprPtr>);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::map<std::string, ExprPtr> Entries;
    };

    struct ConstStringExpr : Expr
    {
        ConstStringExpr(TypePtr, std::string);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::string Value;
    };

    struct FormatExpr : Expr
    {
        FormatExpr(TypePtr, size_t, std::map<size_t, std::string>, std::map<size_t, ExprPtr>);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        size_t Count;
        std::map<size_t, std::string> Statics;
        std::map<size_t, ExprPtr> Dynamics;
    };

    struct MemberExpr : Expr
    {
        MemberExpr(TypePtr, ExprPtr, std::string);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Object;
        std::string Member;
    };

    struct SubscriptExpr : Expr
    {
        SubscriptExpr(TypePtr, ExprPtr, ExprPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Array;
        ExprPtr Index;
    };

    struct SymbolExpr : Expr
    {
        SymbolExpr(TypePtr, std::string);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::string Name;
    };

    struct UnaryExpr : Expr
    {
        UnaryExpr(TypePtr, std::string, bool, ExprPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::string Op;
        bool OpRight;
        ExprPtr Operand;
    };
}
