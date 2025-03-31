#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <set>
#include <string>
#include <newjs/import.hpp>
#include <newjs/macro.hpp>
#include <newjs/newjs.hpp>
#include <newjs/token.hpp>
#include <newjs/type.hpp>

namespace NJS
{
    using Consumer = std::function<void(const StatementPtr &)>;

    class Parser
    {
    public:
        Parser(
            TypeContext &type_context,
            Builder &builder,
            std::istream &stream,
            SourceLocation where,
            std::map<std::string, Macro> &macro_map,
            bool is_main,
            bool is_import = false,
            std::set<std::filesystem::path> parsed_set = {});
        Parser(const Parser &other, std::istream &stream, SourceLocation where);

        [[nodiscard]] SourceLocation CurrentLocation() const;

        void Parse(const Consumer &consumer);

        int Get();
        void UnGet();
        void NewLine();
        int Escape(int c);
        Token &Next();

        [[nodiscard]] bool AtEof() const;
        [[nodiscard]] bool At(TokenType type) const;
        [[nodiscard]] bool At(const std::string &value) const;

        bool NextAt(TokenType type);
        bool NextAt(const std::string &value);

        Token Skip();

        Token Expect(TokenType type);
        Token Expect(const std::string &value);

        TypePtr ParseType();
        TypePtr ParseTupleType();
        TypePtr ParseStructType();
        TypePtr ParseFunctionType();
        TypePtr ParseLambdaType();
        ReferenceInfo ParseReferenceInfo();
        bool ParseTypeList(std::vector<TypePtr> &types, const std::string &delimiter);
        void ParseTypeMap(std::vector<std::pair<std::string, TypePtr>> &types, const std::string &delimiter);
        bool ParseReferenceInfoList(std::vector<ReferenceInfo> &infos, const std::string &delimiter);

        ParameterPtr ParseParameter(bool is_const, bool is_reference);
        bool ParseParameterList(std::vector<ParameterPtr> &parameters, const std::string &delimiter, bool allow_var_arg);
        bool ParseReferenceParameterList(
            std::vector<ParameterPtr> &parameters,
            const std::string &delimiter,
            bool allow_var_arg);
        void ParseParameterMap(std::map<std::string, ParameterPtr> &parameters, const std::string &delimiter);

        void ParseMacro();
        void ParseTypeAlias();

        StatementPtr ParseImportStatement();
        ImportMapping ParseImportMapping();

        StatementPtr ParseStatement();
        StatementPtr ParseBreakStatement();
        StatementPtr ParseClassStatement();
        StatementPtr ParseContinueStatement();
        StatementPtr ParseExportStatement();
        StatementPtr ParseForStatement();
        StatementPtr ParseFunctionStatement(bool is_extern);
        StatementPtr ParseIfStatement();
        StatementPtr ParseReturnStatement();
        StatementPtr ParseScopeStatement();
        StatementPtr ParseSwitchStatement();
        StatementPtr ParseVariableStatement(bool is_extern);
        StatementPtr ParseWhileStatement();

        ExpressionPtr ParseExpression();
        ExpressionPtr ParseAsmExpression();
        ExpressionPtr ParseBinaryExpression(ExpressionPtr lhs, unsigned min_pre);
        ExpressionPtr ParseFormatExpression();
        ExpressionPtr ParseFunctionExpression();
        ExpressionPtr ParseOperandExpression();
        ExpressionPtr ParsePrimaryExpression();
        ExpressionPtr ParseScopeExpression();
        ExpressionPtr ParseStructExpression();
        ExpressionPtr ParseSymbolExpression(const SourceLocation &where, const std::string &name);
        ExpressionPtr ParseSwitchExpression();
        ExpressionPtr ParseTupleExpression();
        ExpressionPtr ParseUnaryExpression();

    private:
        TypeContext &m_TypeContext;
        Builder &m_Builder;

        std::istream &m_Stream;
        std::map<std::string, Macro> &m_MacroMap;
        bool m_IsMain;
        bool m_IsImport;
        std::set<std::filesystem::path> m_ParsedSet;

        SourceLocation m_Where;
        Token m_Token;
    };
}
