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
        FnType_Template,
    };

    struct Stmt
    {
        explicit Stmt(SourceLocation);

        virtual ~Stmt() = default;
        virtual void GenVoidLLVM(Builder&) const = 0;
        virtual std::ostream& Print(std::ostream&) = 0;

        SourceLocation Where;
    };

    struct ForStmt final : Stmt
    {
        ForStmt(SourceLocation, StmtPtr, ExprPtr, StmtPtr, StmtPtr);

        void GenVoidLLVM(Builder&) const override;
        std::ostream& Print(std::ostream&) override;

        StmtPtr Init;
        ExprPtr Condition;
        StmtPtr Loop;
        StmtPtr Body;
    };

    struct FunctionStmt final : Stmt
    {
        FunctionStmt(SourceLocation, bool, FnType, std::string, std::vector<ParamPtr>, bool, TypePtr, StmtPtr);

        void GenVoidLLVM(Builder&) const override;
        std::ostream& Print(std::ostream&) override;

        bool Absolute;
        FnType Fn;
        std::string Name;
        std::vector<ParamPtr> Args;
        bool VarArg;
        TypePtr ResultType;
        StmtPtr Body;
    };

    struct IfStmt final : Stmt
    {
        IfStmt(SourceLocation, ExprPtr, StmtPtr, StmtPtr);

        void GenVoidLLVM(Builder&) const override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Condition;
        StmtPtr Then;
        StmtPtr Else;
    };

    struct ImportStmt final : Stmt
    {
        ImportStmt(SourceLocation, ImportMapping, std::filesystem::path, std::vector<StmtPtr>);

        void GenVoidLLVM(Builder&) const override;
        std::ostream& Print(std::ostream&) override;

        ImportMapping Mapping;
        std::filesystem::path Filepath;
        std::vector<StmtPtr> Functions;
    };

    struct ReturnStmt final : Stmt
    {
        ReturnStmt(SourceLocation, ExprPtr);

        void GenVoidLLVM(Builder&) const override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Value;
    };

    struct ScopeStmt final : Stmt
    {
        ScopeStmt(SourceLocation, std::vector<StmtPtr>);

        void GenVoidLLVM(Builder&) const override;
        std::ostream& Print(std::ostream&) override;

        std::vector<StmtPtr> Children;
    };

    struct SwitchStmt final : Stmt
    {
        SwitchStmt(SourceLocation, ExprPtr, std::map<StmtPtr, std::vector<ExprPtr>>, StmtPtr);

        void GenVoidLLVM(Builder&) const override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Condition;
        std::map<StmtPtr, std::vector<ExprPtr>> Cases;
        StmtPtr DefaultCase;
    };

    struct VariableStmt final : Stmt
    {
        VariableStmt(SourceLocation, bool, ParamPtr, ExprPtr);

        void GenVoidLLVM(Builder&) const override;
        std::ostream& Print(std::ostream&) override;

        bool IsConst;
        ParamPtr Name;
        ExprPtr Value;
    };

    struct Expr : Stmt
    {
        explicit Expr(SourceLocation);

        void GenVoidLLVM(Builder&) const override;
        virtual ValuePtr GenLLVM(Builder&, const TypePtr&) const = 0;
    };

    struct BinaryExpr final : Expr
    {
        BinaryExpr(SourceLocation, std::string, ExprPtr, ExprPtr);

        ValuePtr GenLLVM(Builder&, const TypePtr&) const override;
        std::ostream& Print(std::ostream&) override;

        std::string Operator;
        ExprPtr LeftOperand;
        ExprPtr RightOperand;
    };

    struct BoolExpr final : Expr
    {
        BoolExpr(SourceLocation, bool);

        ValuePtr GenLLVM(Builder&, const TypePtr&) const override;
        std::ostream& Print(std::ostream&) override;

        bool Value;
    };

    struct CallExpr final : Expr
    {
        CallExpr(SourceLocation, ExprPtr, std::vector<ExprPtr>);

        ValuePtr GenLLVM(Builder&, const TypePtr&) const override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Callee;
        std::vector<ExprPtr> Args;
    };

    struct CastExpr final : Expr
    {
        CastExpr(SourceLocation, TypePtr, ExprPtr);

        ValuePtr GenLLVM(Builder&, const TypePtr&) const override;
        std::ostream& Print(std::ostream&) override;

        TypePtr Type;
        ExprPtr Value;
    };

    struct CharExpr final : Expr
    {
        CharExpr(SourceLocation, char);

        ValuePtr GenLLVM(Builder&, const TypePtr&) const override;
        std::ostream& Print(std::ostream&) override;

        char Value;
    };

    struct FormatExpr final : Expr
    {
        FormatExpr(SourceLocation, unsigned, std::map<unsigned, std::string>, std::map<unsigned, ExprPtr>);

        ValuePtr GenLLVM(Builder&, const TypePtr&) const override;
        std::ostream& Print(std::ostream&) override;

        unsigned Count;
        std::map<unsigned, std::string> Statics;
        std::map<unsigned, ExprPtr> Dynamics;
    };

    struct FPExpr final : Expr
    {
        FPExpr(SourceLocation, TypePtr, double);

        ValuePtr GenLLVM(Builder&, const TypePtr&) const override;
        std::ostream& Print(std::ostream&) override;

        TypePtr Type;
        double Value;
    };

    struct FunctionExpr final : Expr
    {
        FunctionExpr(SourceLocation, std::vector<ParamPtr>, bool, TypePtr, StmtPtr);

        ValuePtr GenLLVM(Builder&, const TypePtr&) const override;
        std::ostream& Print(std::ostream&) override;

        std::vector<ParamPtr> Args;
        bool VarArg;
        TypePtr ResultType;
        StmtPtr Body;
    };

    struct IntExpr final : Expr
    {
        IntExpr(SourceLocation, TypePtr, uint64_t);

        ValuePtr GenLLVM(Builder&, const TypePtr&) const override;
        std::ostream& Print(std::ostream&) override;

        TypePtr Type;
        uint64_t Value;
    };

    struct MemberExpr final : Expr
    {
        MemberExpr(SourceLocation, ExprPtr, std::string);

        ValuePtr GenLLVM(Builder&, const TypePtr&) const override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Object;
        std::string Member;
    };

    struct ScopeExpr final : Expr
    {
        ScopeExpr(SourceLocation, std::vector<StmtPtr>, ExprPtr);

        ValuePtr GenLLVM(Builder&, const TypePtr&) const override;
        std::ostream& Print(std::ostream&) override;

        std::vector<StmtPtr> Children;
        ExprPtr Last;
    };

    struct SizeOfExpr final : Expr
    {
        SizeOfExpr(SourceLocation, ExprPtr);

        ValuePtr GenLLVM(Builder&, const TypePtr&) const override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Operand;
    };

    struct StringExpr final : Expr
    {
        StringExpr(SourceLocation, std::string);

        ValuePtr GenLLVM(Builder&, const TypePtr&) const override;
        std::ostream& Print(std::ostream&) override;

        std::string Value;

        static llvm::Constant* GetString(const Builder&, const std::string&);
    };

    struct StructExpr final : Expr
    {
        StructExpr(SourceLocation, std::map<std::string, ExprPtr>);

        ValuePtr GenLLVM(Builder&, const TypePtr&) const override;
        std::ostream& Print(std::ostream&) override;

        std::map<std::string, ExprPtr> Elements;
    };

    struct SubscriptExpr final : Expr
    {
        SubscriptExpr(SourceLocation, ExprPtr, ExprPtr);

        ValuePtr GenLLVM(Builder&, const TypePtr&) const override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Array;
        ExprPtr Index;
    };

    struct SwitchExpr final : Expr
    {
        SwitchExpr(SourceLocation, ExprPtr, std::map<ExprPtr, std::vector<ExprPtr>>, ExprPtr);

        ValuePtr GenLLVM(Builder&, const TypePtr&) const override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Condition;
        std::map<ExprPtr, std::vector<ExprPtr>> Cases;
        ExprPtr DefaultCase;
    };

    struct SymbolExpr final : Expr
    {
        SymbolExpr(SourceLocation, std::string);

        ValuePtr GenLLVM(Builder&, const TypePtr&) const override;
        std::ostream& Print(std::ostream&) override;

        std::string Name;
    };

    struct TernaryExpr final : Expr
    {
        TernaryExpr(SourceLocation, ExprPtr, ExprPtr, ExprPtr);

        ValuePtr GenLLVM(Builder&, const TypePtr&) const override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Condition;
        ExprPtr Then;
        ExprPtr Else;
    };

    struct TupleExpr final : Expr
    {
        TupleExpr(SourceLocation, std::vector<ExprPtr>);

        ValuePtr GenLLVM(Builder&, const TypePtr&) const override;
        std::ostream& Print(std::ostream&) override;

        std::vector<ExprPtr> Elements;
    };

    struct TypeOfExpr final : Expr
    {
        TypeOfExpr(SourceLocation, ExprPtr);

        ValuePtr GenLLVM(Builder&, const TypePtr&) const override;
        std::ostream& Print(std::ostream&) override;

        ExprPtr Operand;
    };

    struct UnaryExpr final : Expr
    {
        UnaryExpr(SourceLocation, std::string, bool, ExprPtr);

        ValuePtr GenLLVM(Builder&, const TypePtr&) const override;
        std::ostream& Print(std::ostream&) override;

        std::string Op;
        bool OpRight;
        ExprPtr Operand;
    };
}
