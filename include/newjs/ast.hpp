#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <set>
#include <vector>
#include <newjs/import.hpp>
#include <newjs/info.hpp>
#include <newjs/location.hpp>
#include <newjs/newjs.hpp>

namespace NJS
{
    struct Statement
    {
        explicit Statement(SourceLocation where);

        void GenLLVM(Builder &builder) const;

        virtual ~Statement() = default;
        virtual std::ostream &Print(std::ostream &stream) const = 0;

        SourceLocation Where;

    protected:
        virtual void PGenLLVM(Builder &builder) const = 0;
    };

    struct BreakStatement final : Statement
    {
        explicit BreakStatement(SourceLocation where);

        std::ostream &Print(std::ostream &stream) const override;

    protected:
        void PGenLLVM(Builder &builder) const override;
    };

    struct ContinueStatement final : Statement
    {
        explicit ContinueStatement(SourceLocation where);

        std::ostream &Print(std::ostream &stream) const override;

    protected:
        void PGenLLVM(Builder &builder) const override;
    };

    struct ForStatement final : Statement
    {
        ForStatement(
            SourceLocation where,
            StatementPtr initializer,
            ExpressionPtr condition,
            StatementPtr loop,
            StatementPtr body);

        std::ostream &Print(std::ostream &stream) const override;

        StatementPtr Initializer;
        ExpressionPtr Condition;
        StatementPtr Loop;
        StatementPtr Body;

    protected:
        void PGenLLVM(Builder &builder) const override;
    };

    struct FunctionStatement final : Statement
    {
        FunctionStatement(
            SourceLocation where,
            unsigned flags,
            std::string name,
            std::vector<ParameterPtr> parameters,
            bool is_var_arg,
            ReferenceInfo result,
            StatementPtr body);

        std::ostream &Print(std::ostream &stream) const override;

        unsigned Flags;
        std::string Name;
        std::vector<ParameterPtr> Parameters;
        bool IsVarArg;
        ReferenceInfo Result;
        StatementPtr Body;

    protected:
        void PGenLLVM(Builder &builder) const override;
    };

    struct IfStatement final : Statement
    {
        IfStatement(SourceLocation where, ExpressionPtr condition, StatementPtr then_body, StatementPtr else_body);

        std::ostream &Print(std::ostream &stream) const override;

        ExpressionPtr Condition;
        StatementPtr ThenBody;
        StatementPtr ElseBody;

    protected:
        void PGenLLVM(Builder &builder) const override;
    };

    struct ImportStatement final : Statement
    {
        ImportStatement(
            SourceLocation where,
            ImportMapping mapping,
            std::filesystem::path filepath,
            std::vector<StatementPtr> values,
            std::string module_id,
            std::set<std::string> sub_module_ids);

        std::ostream &Print(std::ostream &stream) const override;

        ImportMapping Mapping;
        std::filesystem::path Filepath;
        std::vector<StatementPtr> Values;
        std::string ModuleID;
        std::set<std::string> SubModuleIDs;

    protected:
        void PGenLLVM(Builder &builder) const override;
    };

    struct ReturnStatement final : Statement
    {
        ReturnStatement(SourceLocation where, ExpressionPtr value);

        std::ostream &Print(std::ostream &stream) const override;

        ExpressionPtr Value;

    protected:
        void PGenLLVM(Builder &builder) const override;
    };

    struct ScopeStatement final : Statement
    {
        ScopeStatement(SourceLocation where, std::vector<StatementPtr> children);

        std::ostream &Print(std::ostream &stream) const override;

        std::vector<StatementPtr> Children;

    protected:
        void PGenLLVM(Builder &builder) const override;
    };

    struct SwitchStatement final : Statement
    {
        SwitchStatement(
            SourceLocation where,
            ExpressionPtr condition,
            std::map<StatementPtr, std::vector<ExpressionPtr>> cases,
            StatementPtr default_case);

        std::ostream &Print(std::ostream &stream) const override;

        ExpressionPtr Condition;
        std::map<StatementPtr, std::vector<ExpressionPtr>> Cases;
        StatementPtr DefaultCase;

    protected:
        void PGenLLVM(Builder &builder) const override;
    };

    struct VariableStatement final : Statement
    {
        VariableStatement(
            SourceLocation where,
            bool is_export,
            bool is_extern,
            ParameterPtr parameter,
            ExpressionPtr value);

        std::ostream &Print(std::ostream &stream) const override;

        bool IsExport;
        bool IsExtern;
        ParameterPtr Parameter;
        ExpressionPtr Value;

    protected:
        void PGenLLVM(Builder &builder) const override;
    };

    struct WhileStatement final : Statement
    {
        WhileStatement(SourceLocation where, ExpressionPtr condition, StatementPtr body);

        std::ostream &Print(std::ostream &stream) const override;

        ExpressionPtr Condition;
        StatementPtr Body;

    protected:
        void PGenLLVM(Builder &builder) const override;
    };

    struct Expression : Statement
    {
        explicit Expression(SourceLocation where);

        [[nodiscard]] ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const;

    protected:
        void PGenLLVM(Builder &builder) const override;

        [[nodiscard]] virtual ValuePtr PGenLLVM(Builder &builder, const TypePtr &expected_type) const = 0;
    };

    struct AsmExpression final : Expression
    {
        AsmExpression(
            SourceLocation where,
            std::string source,
            std::string constraints,
            bool is_intel,
            bool is_volatile,
            bool is_align,
            bool can_throw,
            std::vector<ExpressionPtr> operands,
            TypePtr output_type);

        std::ostream &Print(std::ostream &stream) const override;

        std::string Source;
        std::string Constraints;
        bool IsIntel;
        bool IsVolatile;
        bool IsAlign;
        bool IsThrow;
        std::vector<ExpressionPtr> Operands;
        TypePtr OutputType;

    protected:
        [[nodiscard]] ValuePtr PGenLLVM(Builder &builder, const TypePtr &expected_type) const override;
    };

    struct BinaryExpression final : Expression
    {
        BinaryExpression(
            SourceLocation where,
            std::string operator_,
            ExpressionPtr left_operand,
            ExpressionPtr right_operand);

        std::ostream &Print(std::ostream &stream) const override;

        std::string Operator;
        ExpressionPtr LeftOperand;
        ExpressionPtr RightOperand;

    protected:
        [[nodiscard]] ValuePtr PGenLLVM(Builder &builder, const TypePtr &expected_type) const override;
    };

    struct BooleanExpression final : Expression
    {
        BooleanExpression(SourceLocation where, bool value);

        std::ostream &Print(std::ostream &stream) const override;

        bool Value;

    protected:
        [[nodiscard]] ValuePtr PGenLLVM(Builder &builder, const TypePtr &expected_type) const override;
    };

    struct CallExpression final : Expression
    {
        CallExpression(SourceLocation where, ExpressionPtr callee, std::vector<ExpressionPtr> arguments);

        std::ostream &Print(std::ostream &stream) const override;

        ExpressionPtr Callee;
        std::vector<ExpressionPtr> Arguments;

    protected:
        [[nodiscard]] ValuePtr PGenLLVM(Builder &builder, const TypePtr &expected_type) const override;
    };

    struct CastExpression final : Expression
    {
        CastExpression(SourceLocation where, TypePtr type, ExpressionPtr operand);

        std::ostream &Print(std::ostream &stream) const override;

        TypePtr Type;
        ExpressionPtr Operand;

    protected:
        [[nodiscard]] ValuePtr PGenLLVM(Builder &builder, const TypePtr &expected_type) const override;
    };

    struct CharExpression final : Expression
    {
        CharExpression(SourceLocation where, char value);

        std::ostream &Print(std::ostream &stream) const override;

        char Value;

    protected:
        [[nodiscard]] ValuePtr PGenLLVM(Builder &builder, const TypePtr &expected_type) const override;
    };

    struct FormatExpression final : Expression
    {
        FormatExpression(
            SourceLocation where,
            unsigned operand_count,
            std::map<unsigned, std::string> static_operands,
            std::map<unsigned, ExpressionPtr> dynamic_operands);

        std::ostream &Print(std::ostream &stream) const override;

        unsigned OperandCount;
        std::map<unsigned, std::string> StaticOperands;
        std::map<unsigned, ExpressionPtr> DynamicOperands;

    protected:
        [[nodiscard]] ValuePtr PGenLLVM(Builder &builder, const TypePtr &expected_type) const override;
    };

    struct FloatingPointExpression final : Expression
    {
        FloatingPointExpression(SourceLocation where, TypePtr type, double value);

        std::ostream &Print(std::ostream &stream) const override;

        TypePtr Type;
        double Value;

    protected:
        [[nodiscard]] ValuePtr PGenLLVM(Builder &builder, const TypePtr &expected_type) const override;
    };

    struct FunctionExpression final : Expression
    {
        FunctionExpression(
            SourceLocation where,
            std::vector<ParameterPtr> parameters,
            bool is_var_arg,
            ReferenceInfo result,
            StatementPtr body);

        std::ostream &Print(std::ostream &stream) const override;

        std::vector<ParameterPtr> Parameters;
        bool IsVarArg;
        ReferenceInfo Result;
        StatementPtr Body;

    protected:
        [[nodiscard]] ValuePtr PGenLLVM(Builder &builder, const TypePtr &expected_type) const override;
    };

    struct IntegerExpression final : Expression
    {
        IntegerExpression(SourceLocation where, TypePtr type, uint64_t value);

        std::ostream &Print(std::ostream &stream) const override;

        TypePtr Type;
        uint64_t Value;

    protected:
        [[nodiscard]] ValuePtr PGenLLVM(Builder &builder, const TypePtr &expected_type) const override;
    };

    struct MemberExpression final : Expression
    {
        MemberExpression(SourceLocation where, ExpressionPtr object, std::string member, bool dereference);

        std::ostream &Print(std::ostream &stream) const override;

        ExpressionPtr Object;
        std::string Member;
        bool Dereference;

    protected:
        [[nodiscard]] ValuePtr PGenLLVM(Builder &builder, const TypePtr &expected_type) const override;
    };

    struct ScopeExpression final : Expression
    {
        ScopeExpression(SourceLocation where, std::vector<StatementPtr> children, ExpressionPtr last);

        std::ostream &Print(std::ostream &stream) const override;

        std::vector<StatementPtr> Children;
        ExpressionPtr Last;

    protected:
        [[nodiscard]] ValuePtr PGenLLVM(Builder &builder, const TypePtr &expected_type) const override;
    };

    struct SizeOfExpression final : Expression
    {
        SizeOfExpression(SourceLocation where, ExpressionPtr operand);

        std::ostream &Print(std::ostream &stream) const override;

        ExpressionPtr Operand;

    protected:
        [[nodiscard]] ValuePtr PGenLLVM(Builder &builder, const TypePtr &expected_type) const override;
    };

    struct StringExpression final : Expression
    {
        StringExpression(SourceLocation where, std::string value);

        std::ostream &Print(std::ostream &stream) const override;

        std::string Value;

    protected:
        [[nodiscard]] ValuePtr PGenLLVM(Builder &builder, const TypePtr &expected_type) const override;
    };

    struct StructExpression final : Expression
    {
        StructExpression(
            SourceLocation where,
            TypePtr type,
            std::vector<std::pair<std::string, ExpressionPtr>> elements);

        std::ostream &Print(std::ostream &stream) const override;

        TypePtr Type;
        std::vector<std::pair<std::string, ExpressionPtr>> Elements;

    protected:
        [[nodiscard]] ValuePtr PGenLLVM(Builder &builder, const TypePtr &expected_type) const override;
    };

    struct SubscriptExpression final : Expression
    {
        SubscriptExpression(SourceLocation where, ExpressionPtr array, ExpressionPtr index);

        std::ostream &Print(std::ostream &stream) const override;

        ExpressionPtr Array;
        ExpressionPtr Index;

    protected:
        [[nodiscard]] ValuePtr PGenLLVM(Builder &builder, const TypePtr &expected_type) const override;
    };

    struct SwitchExpression final : Expression
    {
        SwitchExpression(
            SourceLocation where,
            ExpressionPtr condition,
            std::map<ExpressionPtr, std::vector<ExpressionPtr>> cases,
            ExpressionPtr default_case);

        std::ostream &Print(std::ostream &stream) const override;

        ExpressionPtr Condition;
        std::map<ExpressionPtr, std::vector<ExpressionPtr>> Cases;
        ExpressionPtr DefaultCase;

    protected:
        [[nodiscard]] ValuePtr PGenLLVM(Builder &builder, const TypePtr &expected_type) const override;
    };

    struct SymbolExpression final : Expression
    {
        SymbolExpression(SourceLocation where, std::string name);

        std::ostream &Print(std::ostream &stream) const override;

        std::string Name;

    protected:
        [[nodiscard]] ValuePtr PGenLLVM(Builder &builder, const TypePtr &expected_type) const override;
    };

    struct TernaryExpression final : Expression
    {
        TernaryExpression(
            SourceLocation where,
            ExpressionPtr condition,
            ExpressionPtr then_body,
            ExpressionPtr else_body);

        std::ostream &Print(std::ostream &stream) const override;

        ExpressionPtr Condition;
        ExpressionPtr ThenBody;
        ExpressionPtr ElseBody;

    protected:
        [[nodiscard]] ValuePtr PGenLLVM(Builder &builder, const TypePtr &expected_type) const override;
    };

    struct TupleExpression final : Expression
    {
        TupleExpression(SourceLocation where, TypePtr type, std::vector<ExpressionPtr> elements);

        std::ostream &Print(std::ostream &stream) const override;

        TypePtr Type;
        std::vector<ExpressionPtr> Elements;

    protected:
        [[nodiscard]] ValuePtr PGenLLVM(Builder &builder, const TypePtr &expected_type) const override;
    };

    struct TypeOfExpression final : Expression
    {
        TypeOfExpression(SourceLocation where, ExpressionPtr operand);

        std::ostream &Print(std::ostream &stream) const override;

        ExpressionPtr Operand;

    protected:
        [[nodiscard]] ValuePtr PGenLLVM(Builder &builder, const TypePtr &expected_type) const override;
    };

    struct UnaryExpression final : Expression
    {
        UnaryExpression(SourceLocation where, std::string operator_, bool prefix, ExpressionPtr operand);

        std::ostream &Print(std::ostream &stream) const override;

        std::string Operator;
        bool Prefix;
        ExpressionPtr Operand;

    protected:
        [[nodiscard]] ValuePtr PGenLLVM(Builder &builder, const TypePtr &expected_type) const override;
    };
}
