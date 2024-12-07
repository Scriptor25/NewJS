#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <vector>
#include <llvm/IR/Constant.h>
#include <NJS/Import.hpp>
#include <NJS/NJS.hpp>
#include <NJS/SourceLocation.hpp>

namespace NJS
{
    struct Stmt
    {
        explicit Stmt(SourceLocation);

        virtual ~Stmt() = default;
        virtual ValuePtr GenLLVM(Builder&) = 0;
        virtual std::ostream& Print(std::ostream&) = 0;

        SourceLocation Where;
    };

    struct ImportStmt : Stmt
    {
        ImportStmt(SourceLocation, ImportMapping, std::filesystem::path, std::vector<FunctionStmtPtr>);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        ImportMapping Mapping;
        std::filesystem::path Filepath;
        std::vector<FunctionStmtPtr> Functions;
    };

    struct ScopeStmt : Stmt
    {
        ScopeStmt(SourceLocation, std::vector<StmtPtr>);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::vector<StmtPtr> Children;
    };

    struct FunctionStmt : Stmt
    {
        FunctionStmt(SourceLocation, bool, std::string, std::vector<ParamPtr>, bool, TypePtr, ScopeStmtPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        bool Extern;
        std::string Name;
        std::vector<ParamPtr> Params;
        bool VarArg;
        TypePtr ResultType;
        ScopeStmtPtr Body;
    };

    struct IfStmt : Stmt
    {
        IfStmt(SourceLocation, ExprPtr, StmtPtr, StmtPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Condition;
        StmtPtr Then;
        StmtPtr Else;
    };

    struct ForStmt : Stmt
    {
        ForStmt(SourceLocation, StmtPtr, ExprPtr, StmtPtr, StmtPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        StmtPtr Init;
        ExprPtr Condition;
        StmtPtr Loop;
        StmtPtr Body;
    };

    struct VariableStmt : Stmt
    {
        VariableStmt(SourceLocation, bool, ParamPtr, ExprPtr);

        void GenLLVM(Builder&, const ValuePtr&) const;

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        bool IsConst;
        ParamPtr Name;
        ExprPtr Value;
    };

    struct ForInOfStmt : Stmt
    {
        ForInOfStmt(SourceLocation, VariableStmt, bool, ExprPtr, StmtPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        VariableStmt Init;
        bool Of;
        ExprPtr Value;
        StmtPtr Body;
    };

    struct ReturnStmt : Stmt
    {
        ReturnStmt(SourceLocation, ExprPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Value;
    };

    struct Expr : Stmt
    {
        Expr(SourceLocation, TypePtr);

        TypePtr Type;
    };

    struct BinaryExpr : Expr
    {
        BinaryExpr(SourceLocation, TypePtr, std::string, ExprPtr, ExprPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::string Op;
        ExprPtr Lhs;
        ExprPtr Rhs;
    };

    struct CallExpr : Expr
    {
        CallExpr(SourceLocation, TypePtr, ExprPtr, std::vector<ExprPtr>);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Callee;
        std::vector<ExprPtr> Args;
    };

    struct ConstBooleanExpr : Expr
    {
        ConstBooleanExpr(SourceLocation, TypePtr, bool);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        bool Value;
    };

    struct ConstCharExpr : Expr
    {
        ConstCharExpr(SourceLocation, TypePtr, char);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        char Value;
    };

    struct ConstFunctionExpr : Expr
    {
        ConstFunctionExpr(SourceLocation, TypePtr, std::vector<ParamPtr>, bool, TypePtr, ScopeStmt);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::vector<ParamPtr> Params;
        bool VarArg;
        TypePtr ResultType;
        ScopeStmt Body;
    };

    struct ConstNumberExpr : Expr
    {
        ConstNumberExpr(SourceLocation, TypePtr, double);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        double Value;
    };

    struct ConstObjectExpr : Expr
    {
        ConstObjectExpr(SourceLocation, TypePtr, std::map<std::string, ExprPtr>);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::map<std::string, ExprPtr> Elements;
    };

    struct ConstStringExpr : Expr
    {
        ConstStringExpr(SourceLocation, TypePtr, std::string);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::string Value;

        static llvm::Constant* GetString(const Builder&, const std::string&);
    };

    struct ConstTupleExpr : Expr
    {
        ConstTupleExpr(SourceLocation, TypePtr, std::vector<ExprPtr>);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::vector<ExprPtr> Elements;
    };

    struct FormatExpr : Expr
    {
        FormatExpr(SourceLocation, TypePtr, size_t, std::map<size_t, std::string>, std::map<size_t, ExprPtr>);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        size_t Count;
        std::map<size_t, std::string> Statics;
        std::map<size_t, ExprPtr> Dynamics;
    };

    struct MemberExpr : Expr
    {
        MemberExpr(SourceLocation, TypePtr, ExprPtr, std::string);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Object;
        std::string Member;
    };

    struct ScopeExpr : Expr
    {
        ScopeExpr(SourceLocation, TypePtr, std::vector<StmtPtr>, ExprPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::vector<StmtPtr> Children;
        ExprPtr Last;
    };

    struct SubscriptExpr : Expr
    {
        SubscriptExpr(SourceLocation, TypePtr, ExprPtr, ExprPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Array;
        ExprPtr Index;
    };

    struct SwitchExpr : Expr
    {
        SwitchExpr(SourceLocation, TypePtr, ExprPtr, std::map<ExprPtr, std::vector<ExprPtr>>, ExprPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Condition;
        std::map<ExprPtr, std::vector<ExprPtr>> Cases;
        ExprPtr DefaultCase;
    };

    struct SymbolExpr : Expr
    {
        SymbolExpr(SourceLocation, TypePtr, std::string);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::string Name;
    };

    struct TernaryExpr : Expr
    {
        TernaryExpr(SourceLocation, TypePtr, ExprPtr, ExprPtr, ExprPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Condition;
        ExprPtr Then;
        ExprPtr Else;
    };

    struct UnaryExpr : Expr
    {
        UnaryExpr(SourceLocation, TypePtr, std::string, bool, ExprPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::string Op;
        bool OpRight;
        ExprPtr Operand;
    };
}
