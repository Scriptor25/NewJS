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
    enum FnType
    {
        FnType_Function,
        FnType_Extern,
        FnType_Operator,
    };

    struct Stmt
    {
        explicit Stmt(SourceLocation);

        virtual ~Stmt() = default;
        virtual ValuePtr GenLLVM(Builder&) = 0;
        virtual std::ostream& Print(std::ostream&) = 0;

        SourceLocation Where;
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

    struct FunctionStmt : Stmt
    {
        FunctionStmt(SourceLocation, FnType, std::string, std::vector<ParamPtr>, bool, TypePtr, StmtPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        FnType Fn;
        std::string Name;
        std::vector<ParamPtr> Args;
        bool VarArg;
        TypePtr ResultType;
        StmtPtr Body;
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

    struct ImportStmt : Stmt
    {
        ImportStmt(SourceLocation, ImportMapping, std::filesystem::path, std::vector<StmtPtr>);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        ImportMapping Mapping;
        std::filesystem::path Filepath;
        std::vector<StmtPtr> Functions;
    };

    struct ReturnStmt : Stmt
    {
        ReturnStmt(SourceLocation, ExprPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Value;
    };

    struct ScopeStmt : Stmt
    {
        ScopeStmt(SourceLocation, std::vector<StmtPtr>);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::vector<StmtPtr> Children;
    };

    struct SwitchStmt : Stmt
    {
        SwitchStmt(SourceLocation, ExprPtr, std::map<StmtPtr, std::vector<ExprPtr>>, StmtPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Condition;
        std::map<StmtPtr, std::vector<ExprPtr>> Cases;
        StmtPtr DefaultCase;
    };

    struct VariableStmt : Stmt
    {
        VariableStmt(SourceLocation, bool, ParamPtr, ExprPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        bool IsConst;
        ParamPtr Name;
        ExprPtr Value;
    };

    struct Expr : Stmt
    {
        explicit Expr(SourceLocation);
    };

    struct BinaryExpr : Expr
    {
        BinaryExpr(SourceLocation, std::string, ExprPtr, ExprPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::string Op;
        ExprPtr Lhs;
        ExprPtr Rhs;
    };

    struct CallExpr : Expr
    {
        CallExpr(SourceLocation, ExprPtr, std::vector<ExprPtr>);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Callee;
        std::vector<ExprPtr> Args;
    };

    struct CastExpr : Expr
    {
        CastExpr(SourceLocation, TypePtr, ExprPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        TypePtr Type;
        ExprPtr Value;
    };

    struct BoolExpr : Expr
    {
        BoolExpr(SourceLocation, bool);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        bool Value;
    };

    struct CharExpr : Expr
    {
        CharExpr(SourceLocation, char);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        char Value;
    };

    struct FPExpr : Expr
    {
        FPExpr(SourceLocation, TypePtr, double);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        TypePtr Type;
        double Value;
    };

    struct FunctionExpr : Expr
    {
        FunctionExpr(SourceLocation, std::vector<ParamPtr>, bool, TypePtr, StmtPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::vector<ParamPtr> Args;
        bool VarArg;
        TypePtr ResultType;
        StmtPtr Body;
    };

    struct IntExpr : Expr
    {
        IntExpr(SourceLocation, TypePtr, uint64_t);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        TypePtr Type;
        uint64_t Value;
    };

    struct StructExpr : Expr
    {
        StructExpr(SourceLocation, std::map<std::string, ExprPtr>);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::map<std::string, ExprPtr> Elements;
    };

    struct StringExpr : Expr
    {
        StringExpr(SourceLocation, std::string);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::string Value;

        static llvm::Constant* GetString(const Builder&, const std::string&);
    };

    struct TupleExpr : Expr
    {
        TupleExpr(SourceLocation, std::vector<ExprPtr>);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::vector<ExprPtr> Elements;
    };

    struct FormatExpr : Expr
    {
        FormatExpr(SourceLocation, unsigned, std::map<unsigned, std::string>, std::map<unsigned, ExprPtr>);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        unsigned Count;
        std::map<unsigned, std::string> Statics;
        std::map<unsigned, ExprPtr> Dynamics;
    };

    struct MemberExpr : Expr
    {
        MemberExpr(SourceLocation, ExprPtr, std::string);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Object;
        std::string Member;
    };

    struct ScopeExpr : Expr
    {
        ScopeExpr(SourceLocation, std::vector<StmtPtr>, ExprPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::vector<StmtPtr> Children;
        ExprPtr Last;
    };

    struct SubscriptExpr : Expr
    {
        SubscriptExpr(SourceLocation, ExprPtr, ExprPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Array;
        ExprPtr Index;
    };

    struct SwitchExpr : Expr
    {
        SwitchExpr(SourceLocation, ExprPtr, std::map<ExprPtr, std::vector<ExprPtr>>, ExprPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Condition;
        std::map<ExprPtr, std::vector<ExprPtr>> Cases;
        ExprPtr DefaultCase;
    };

    struct SymbolExpr : Expr
    {
        SymbolExpr(SourceLocation, std::string);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::string Name;
    };

    struct TernaryExpr : Expr
    {
        TernaryExpr(SourceLocation, ExprPtr, ExprPtr, ExprPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Condition;
        ExprPtr Then;
        ExprPtr Else;
    };

    struct UnaryExpr : Expr
    {
        UnaryExpr(SourceLocation, std::string, bool, ExprPtr);

        ValuePtr GenLLVM(Builder&) override;
        std::ostream& Print(std::ostream&) override;

        std::string Op;
        bool OpRight;
        ExprPtr Operand;
    };
}
