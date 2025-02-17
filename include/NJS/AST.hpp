#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <set>
#include <vector>
#include <llvm/IR/Constant.h>
#include <NJS/Import.hpp>
#include <NJS/NJS.hpp>
#include <NJS/SourceLocation.hpp>

namespace NJS
{
    enum FunctionFlags
    {
        FunctionFlags_None = 0,
        FunctionFlags_Extern = 1,
        FunctionFlags_Operator = 2,
        FunctionFlags_Template = 4,
        FunctionFlags_Absolute = 8,
    };

    struct Statement
    {
        explicit Statement(SourceLocation where);

        virtual ~Statement() = default;
        virtual void GenVoidLLVM(Builder &builder) const = 0;
        virtual std::ostream &Print(std::ostream &stream) = 0;

        SourceLocation Where;
    };

    struct ForStatement final : Statement
    {
        ForStatement(
            SourceLocation where,
            StatementPtr initializer,
            ExpressionPtr condition,
            StatementPtr loop,
            StatementPtr body);

        void GenVoidLLVM(Builder &builder) const override;
        std::ostream &Print(std::ostream &stream) override;

        StatementPtr Initializer;
        ExpressionPtr Condition;
        StatementPtr Loop;
        StatementPtr Body;
    };

    struct FunctionStatement final : Statement
    {
        FunctionStatement(
            SourceLocation where,
            unsigned flags,
            std::string name,
            std::vector<ParameterPtr> parameters,
            bool is_var_arg,
            TypePtr result_type,
            StatementPtr body);

        void GenVoidLLVM(Builder &builder) const override;
        std::ostream &Print(std::ostream &stream) override;

        unsigned Flags;
        std::string Name;
        std::vector<ParameterPtr> Parameters;
        bool IsVarArg;
        TypePtr ResultType;
        StatementPtr Body;
    };

    struct IfStatement final : Statement
    {
        IfStatement(SourceLocation where, ExpressionPtr condition, StatementPtr then_body, StatementPtr else_body);

        void GenVoidLLVM(Builder &builder) const override;
        std::ostream &Print(std::ostream &stream) override;

        ExpressionPtr Condition;
        StatementPtr ThenBody;
        StatementPtr ElseBody;
    };

    struct ImportStatement final : Statement
    {
        ImportStatement(
            SourceLocation where,
            ImportMapping mapping,
            std::filesystem::path filepath,
            std::vector<StatementPtr> functions,
            std::string module_id,
            std::set<std::string> sub_module_ids);

        static void CreateModuleCall(const Builder &builder, const std::string &module_id);

        void GenVoidLLVM(Builder &builder) const override;
        std::ostream &Print(std::ostream &stream) override;

        ImportMapping Mapping;
        std::filesystem::path Filepath;
        std::vector<StatementPtr> Functions;
        std::string ModuleID;
        std::set<std::string> SubModuleIDs;
    };

    struct ReturnStatement final : Statement
    {
        ReturnStatement(SourceLocation where, ExpressionPtr value);

        void GenVoidLLVM(Builder &builder) const override;
        std::ostream &Print(std::ostream &stream) override;

        ExpressionPtr Value;
    };

    struct ScopeStatement final : Statement
    {
        ScopeStatement(SourceLocation where, std::vector<StatementPtr> children);

        void GenVoidLLVM(Builder &builder) const override;
        std::ostream &Print(std::ostream &stream) override;

        std::vector<StatementPtr> Children;
    };

    struct SwitchStatement final : Statement
    {
        SwitchStatement(
            SourceLocation where,
            ExpressionPtr condition,
            std::map<StatementPtr, std::vector<ExpressionPtr>> cases,
            StatementPtr default_case);

        void GenVoidLLVM(Builder &builder) const override;
        std::ostream &Print(std::ostream &stream) override;

        ExpressionPtr Condition;
        std::map<StatementPtr, std::vector<ExpressionPtr>> Cases;
        StatementPtr DefaultCase;
    };

    struct VariableStatement final : Statement
    {
        VariableStatement(SourceLocation where, ParameterPtr name, unsigned flags, ExpressionPtr value);

        void GenVoidLLVM(Builder &builder) const override;
        std::ostream &Print(std::ostream &stream) override;

        ParameterPtr Name;
        unsigned Flags;
        ExpressionPtr Value;
    };

    struct WhileStatement final : Statement
    {
        WhileStatement(SourceLocation where, ExpressionPtr condition, StatementPtr body);

        void GenVoidLLVM(Builder &builder) const override;
        std::ostream &Print(std::ostream &stream) override;

        ExpressionPtr Condition;
        StatementPtr Body;
    };

    struct Expression : Statement
    {
        explicit Expression(SourceLocation where);

        void GenVoidLLVM(Builder &builder) const override;
        virtual ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const = 0;
    };

    struct BinaryExpression final : Expression
    {
        BinaryExpression(
            SourceLocation where,
            std::string operator_,
            ExpressionPtr left_operand,
            ExpressionPtr right_operand);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        std::string Operator;
        ExpressionPtr LeftOperand;
        ExpressionPtr RightOperand;
    };

    struct BooleanExpression final : Expression
    {
        BooleanExpression(SourceLocation where, bool value);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        bool Value;
    };

    struct CallExpression final : Expression
    {
        CallExpression(SourceLocation where, ExpressionPtr callee, std::vector<ExpressionPtr> arguments);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        ExpressionPtr Callee;
        std::vector<ExpressionPtr> Arguments;
    };

    struct CastExpression final : Expression
    {
        CastExpression(SourceLocation where, TypePtr type, ExpressionPtr operand);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        TypePtr Type;
        ExpressionPtr Operand;
    };

    struct CharacterExpression final : Expression
    {
        CharacterExpression(SourceLocation where, char value);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        char Value;
    };

    struct FormatExpression final : Expression
    {
        FormatExpression(
            SourceLocation where,
            unsigned count,
            std::map<unsigned, std::string> static_expressions,
            std::map<unsigned, ExpressionPtr> dynamic_expressions);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        unsigned Count;
        std::map<unsigned, std::string> StaticExpressions;
        std::map<unsigned, ExpressionPtr> DynamicExpressions;
    };

    struct FloatingPointExpression final : Expression
    {
        FloatingPointExpression(SourceLocation where, TypePtr type, double value);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        TypePtr Type;
        double Value;
    };

    struct FunctionExpression final : Expression
    {
        FunctionExpression(
            SourceLocation where,
            std::vector<ParameterPtr> parameters,
            bool is_var_arg,
            TypePtr result_type,
            StatementPtr body);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        std::vector<ParameterPtr> Parameters;
        bool IsVarArg;
        TypePtr ResultType;
        StatementPtr Body;
    };

    struct IntegerExpression final : Expression
    {
        IntegerExpression(SourceLocation where, TypePtr type, uint64_t value);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        TypePtr Type;
        uint64_t Value;
    };

    struct MemberExpression final : Expression
    {
        MemberExpression(SourceLocation where, ExpressionPtr object, std::string member);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        ExpressionPtr Object;
        std::string Member;
    };

    struct ScopeExpression final : Expression
    {
        ScopeExpression(SourceLocation where, std::vector<StatementPtr> children, ExpressionPtr last);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        std::vector<StatementPtr> Children;
        ExpressionPtr Last;
    };

    struct SizeOfExpression final : Expression
    {
        SizeOfExpression(SourceLocation where, ExpressionPtr operand);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        ExpressionPtr Operand;
    };

    struct StringExpression final : Expression
    {
        static llvm::Constant *GetString(const Builder &builder, const std::string &value);

        StringExpression(SourceLocation where, std::string value);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        std::string Value;
    };

    struct StructExpression final : Expression
    {
        StructExpression(SourceLocation where, std::map<std::string, ExpressionPtr> elements);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        std::map<std::string, ExpressionPtr> Elements;
    };

    struct SubscriptExpression final : Expression
    {
        SubscriptExpression(SourceLocation where, ExpressionPtr array, ExpressionPtr index);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        ExpressionPtr Array;
        ExpressionPtr Index;
    };

    struct SwitchExpression final : Expression
    {
        SwitchExpression(
            SourceLocation where,
            ExpressionPtr condition,
            std::map<ExpressionPtr, std::vector<ExpressionPtr>> cases,
            ExpressionPtr default_case);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        ExpressionPtr Condition;
        std::map<ExpressionPtr, std::vector<ExpressionPtr>> Cases;
        ExpressionPtr DefaultCase;
    };

    struct SymbolExpression final : Expression
    {
        SymbolExpression(SourceLocation where, std::string name);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        std::string Name;
    };

    struct TernaryExpression final : Expression
    {
        TernaryExpression(
            SourceLocation where,
            ExpressionPtr condition,
            ExpressionPtr then_body,
            ExpressionPtr else_body);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        ExpressionPtr Condition;
        ExpressionPtr ThenBody;
        ExpressionPtr ElseBody;
    };

    struct TupleExpression final : Expression
    {
        TupleExpression(SourceLocation where, std::vector<ExpressionPtr> elements);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        std::vector<ExpressionPtr> Elements;
    };

    struct TypeOfExpression final : Expression
    {
        TypeOfExpression(SourceLocation where, ExpressionPtr operand);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        ExpressionPtr Operand;
    };

    struct UnaryExpression final : Expression
    {
        UnaryExpression(SourceLocation where, std::string operator_, bool prefix, ExpressionPtr operand);

        ValuePtr GenLLVM(Builder &builder, const TypePtr &expected_type) const override;
        std::ostream &Print(std::ostream &stream) override;

        std::string Operator;
        bool Prefix;
        ExpressionPtr Operand;
    };
}
