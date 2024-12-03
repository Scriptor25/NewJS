#pragma once

#include <map>
#include <memory>
#include <vector>
#include <llvm/IR/Constant.h>
#include <NJS/NJS.hpp>

namespace NJS
{
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
        FunctionStmt(std::string name, std::vector<ParamPtr> params, bool, TypePtr, ScopeStmtPtr body);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::string Name;
        std::vector<ParamPtr> Params;
        bool VarArg;
        TypePtr ResultType;
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
    };

    struct BinaryExpr : Expr
    {
        BinaryExpr(std::string, ExprPtr, ExprPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::string Op;
        ExprPtr Lhs;
        ExprPtr Rhs;
    };

    struct CallExpr : Expr
    {
        CallExpr(ExprPtr, std::vector<ExprPtr>);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Callee;
        std::vector<ExprPtr> Args;
    };

    struct ConstTupleExpr : Expr
    {
        explicit ConstTupleExpr(std::vector<ExprPtr>);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::vector<ExprPtr> Elements;
    };

    struct ConstFunctionExpr : Expr
    {
        ConstFunctionExpr(std::vector<ParamPtr>, bool, TypePtr, ScopeStmt);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::vector<ParamPtr> Params;
        bool VarArg;
        TypePtr ResultType;
        ScopeStmt Body;
    };

    struct ConstNumberExpr : Expr
    {
        explicit ConstNumberExpr(double);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        double Value;
    };

    struct ConstObjectExpr : Expr
    {
        explicit ConstObjectExpr(std::map<std::string, ExprPtr>);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::map<std::string, ExprPtr> Elements;
    };

    struct ConstStringExpr : Expr
    {
        explicit ConstStringExpr(std::string);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::string Value;

        static std::map<std::string, llvm::Constant*> GlobalStringTable;
    };

    struct FormatExpr : Expr
    {
        FormatExpr(size_t, std::map<size_t, std::string>, std::map<size_t, ExprPtr>);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        size_t Count;
        std::map<size_t, std::string> Statics;
        std::map<size_t, ExprPtr> Dynamics;
    };

    struct MemberExpr : Expr
    {
        MemberExpr(ExprPtr, std::string);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Object;
        std::string Member;
    };

    struct SubscriptExpr : Expr
    {
        SubscriptExpr(ExprPtr, ExprPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Array;
        ExprPtr Index;
    };

    struct SymbolExpr : Expr
    {
        explicit SymbolExpr(std::string);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::string Name;
    };

    struct UnaryExpr : Expr
    {
        UnaryExpr(std::string, bool, ExprPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::string Op;
        bool OpRight;
        ExprPtr Operand;
    };
}
