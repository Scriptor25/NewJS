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
    enum FunctionID
    {
        FnType_Function,
        FnType_Extern,
        FnType_Operator,
        FnType_Template,
    };

    struct Stmt
    {
        explicit Stmt(SourceLocation where);

        virtual ~Stmt() = default;
        virtual void GenVoidLLVM(Builder &builder) const = 0;
        virtual std::ostream &Print(std::ostream &stream) = 0;

        SourceLocation Where;
    };

    struct ForStmt final : Stmt
    {
        ForStmt(SourceLocation where, StmtPtr initializer, ExprPtr condition, StmtPtr loop, StmtPtr body);

        void GenVoidLLVM(Builder &builder) const override;
        std::ostream &Print(std::ostream &stream) override;

        StmtPtr Initializer;
        ExprPtr Condition;
        StmtPtr Loop;
        StmtPtr Body;
    };

    struct FunctionStmt final : Stmt
    {
        FunctionStmt(
            SourceLocation where,
            bool absolute,
            FunctionID fn_id,
            std::string name,
            std::vector<ParamPtr> args,
            bool var_arg,
            TypePtr result_type,
            StmtPtr body);

        void GenVoidLLVM(Builder &builder) const override;
        std::ostream &Print(std::ostream &stream) override;

        bool Absolute;
        FunctionID FnID;
        std::string Name;
        std::vector<ParamPtr> Args;
        bool VarArg;
        TypePtr ResultType;
        StmtPtr Body;
    };

    struct IfStmt final : Stmt
    {
        IfStmt(SourceLocation where, ExprPtr condition, StmtPtr then_body, StmtPtr else_body);

        void GenVoidLLVM(Builder &builder) const override;
        std::ostream &Print(std::ostream &stream) override;

        ExprPtr Condition;
        StmtPtr ThenBody;
        StmtPtr ElseBody;
    };

    struct ImportStmt final : Stmt
    {
        ImportStmt(
            SourceLocation where,
            ImportMapping mapping,
            std::filesystem::path filepath,
            std::vector<StmtPtr> functions);

        void GenVoidLLVM(Builder &builder) const override;
        std::ostream &Print(std::ostream &stream) override;

        ImportMapping Mapping;
        std::filesystem::path Filepath;
        std::vector<StmtPtr> Functions;
    };

    struct ReturnStmt final : Stmt
    {
        ReturnStmt(SourceLocation where, ExprPtr value);

        void GenVoidLLVM(Builder &builder) const override;
        std::ostream &Print(std::ostream &stream) override;

        ExprPtr Value;
    };

    struct ScopeStmt final : Stmt
    {
        ScopeStmt(SourceLocation where, std::vector<StmtPtr> children);

        void GenVoidLLVM(Builder &builder) const override;
        std::ostream &Print(std::ostream &stream) override;

        std::vector<StmtPtr> Children;
    };

    struct SwitchStmt final : Stmt
    {
        SwitchStmt(
            SourceLocation where,
            ExprPtr condition,
            std::map<StmtPtr, std::vector<ExprPtr> > cases,
            StmtPtr default_case);

        void GenVoidLLVM(Builder &builder) const override;
        std::ostream &Print(std::ostream &stream) override;

        ExprPtr Condition;
        std::map<StmtPtr, std::vector<ExprPtr> > Cases;
        StmtPtr DefaultCase;
    };

    struct VariableStmt final : Stmt
    {
        VariableStmt(SourceLocation where, bool is_const, ParamPtr name, ExprPtr value);

        void GenVoidLLVM(Builder &builder) const override;
        std::ostream &Print(std::ostream &stream) override;

        bool IsConst;
        ParamPtr Name;
        ExprPtr Value;
    };

    struct Expr : Stmt
    {
        explicit Expr(SourceLocation where);

        void GenVoidLLVM(Builder &builder) const override;
        virtual ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const = 0;
    };

    struct BinaryExpr final : Expr
    {
        BinaryExpr(SourceLocation where, std::string_view operator_, ExprPtr left_operand, ExprPtr right_operand);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        std::string Operator;
        ExprPtr LeftOperand;
        ExprPtr RightOperand;
    };

    struct BoolExpr final : Expr
    {
        BoolExpr(SourceLocation where, bool value);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        bool Value;
    };

    struct CallExpr final : Expr
    {
        CallExpr(SourceLocation where, ExprPtr callee, std::vector<ExprPtr> args);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        ExprPtr Callee;
        std::vector<ExprPtr> Args;
    };

    struct CastExpr final : Expr
    {
        CastExpr(SourceLocation where, TypePtr type, ExprPtr value);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        TypePtr Type;
        ExprPtr Value;
    };

    struct CharExpr final : Expr
    {
        CharExpr(SourceLocation where, char value);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        char Value;
    };

    struct FormatExpr final : Expr
    {
        FormatExpr(
            SourceLocation where,
            unsigned count,
            std::map<unsigned, std::string> static_expr,
            std::map<unsigned, ExprPtr> dynamic_expr);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        unsigned Count;
        std::map<unsigned, std::string> StaticExpr;
        std::map<unsigned, ExprPtr> DynamicExpr;
    };

    struct FloatingPointExpr final : Expr
    {
        FloatingPointExpr(SourceLocation where, TypePtr type, double value);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        TypePtr Type;
        double Value;
    };

    struct FunctionExpr final : Expr
    {
        FunctionExpr(SourceLocation where, std::vector<ParamPtr>, bool, TypePtr, StmtPtr);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        std::vector<ParamPtr> Args;
        bool VarArg;
        TypePtr ResultType;
        StmtPtr Body;
    };

    struct IntExpr final : Expr
    {
        IntExpr(SourceLocation where, TypePtr, uint64_t);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        TypePtr Type;
        uint64_t Value;
    };

    struct MemberExpr final : Expr
    {
        MemberExpr(SourceLocation where, ExprPtr object, std::string_view member);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        ExprPtr Object;
        std::string Member;
    };

    struct ScopeExpr final : Expr
    {
        ScopeExpr(SourceLocation where, std::vector<StmtPtr> children, ExprPtr last);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        std::vector<StmtPtr> Children;
        ExprPtr Last;
    };

    struct SizeOfExpr final : Expr
    {
        SizeOfExpr(SourceLocation where, ExprPtr operand);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        ExprPtr Operand;
    };

    struct StringExpr final : Expr
    {
        StringExpr(SourceLocation where, std::string_view value);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        std::string Value;

        static llvm::Constant *GetString(const Builder &builder, const std::string_view &value);
    };

    struct StructExpr final : Expr
    {
        StructExpr(SourceLocation where, std::map<std::string, ExprPtr> elements);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        std::map<std::string, ExprPtr> Elements;
    };

    struct SubscriptExpr final : Expr
    {
        SubscriptExpr(SourceLocation where, ExprPtr array, ExprPtr index);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        ExprPtr Array;
        ExprPtr Index;
    };

    struct SwitchExpr final : Expr
    {
        SwitchExpr(
            SourceLocation where,
            ExprPtr condition,
            std::map<ExprPtr, std::vector<ExprPtr> > cases,
            ExprPtr default_case);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        ExprPtr Condition;
        std::map<ExprPtr, std::vector<ExprPtr> > Cases;
        ExprPtr DefaultCase;
    };

    struct SymbolExpr final : Expr
    {
        SymbolExpr(SourceLocation where, std::string_view name);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        std::string Name;
    };

    struct TernaryExpr final : Expr
    {
        TernaryExpr(SourceLocation where, ExprPtr condition, ExprPtr then_body, ExprPtr else_body);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        ExprPtr Condition;
        ExprPtr ThenBody;
        ExprPtr ElseBody;
    };

    struct TupleExpr final : Expr
    {
        TupleExpr(SourceLocation where, std::vector<ExprPtr> elements);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        std::vector<ExprPtr> Elements;
    };

    struct TypeOfExpr final : Expr
    {
        TypeOfExpr(SourceLocation where, ExprPtr operand);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        ExprPtr Operand;
    };

    struct UnaryExpr final : Expr
    {
        UnaryExpr(SourceLocation where, std::string_view operator_, bool post, ExprPtr operand);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        std::string Operator;
        bool Post;
        ExprPtr Operand;
    };
}
