#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <NJS/Import.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Token.hpp>

namespace NJS
{
    using Consumer = std::function<void(const StmtPtr &stmt)>;

    struct Macro
    {
        std::string Source;
    };

    class Parser
    {
        friend TemplateContext;

    public:
        Parser(
            TypeContext &type_context,
            TemplateContext &template_context,
            std::istream &stream,
            SourceLocation where,
            std::map<std::string, Macro> &macro_map,
            bool is_import = false,
            std::set<std::filesystem::path> parsed_set = {});

        void Parse(const Consumer &consumer);

    private:
        void ResetBuffer();

        int Get();
        void NewLine();
        void Escape();
        Token &Next();

        [[nodiscard]] bool AtEof() const;
        [[nodiscard]] bool At(TokenType type) const;
        [[nodiscard]] bool At(const std::string_view &value) const;

        bool NextAt(TokenType type);
        bool NextAt(const std::string_view &value);

        Token Skip();

        Token Expect(TokenType type);
        Token Expect(const std::string_view &value);

        TypePtr ParseType();
        TypePtr ParseTupleType();
        TypePtr ParseStructType();
        TypePtr ParseFunctionType();
        bool ParseTypeList(std::vector<TypePtr> &types, const std::string_view &delim);
        void ParseTypeMap(std::map<std::string, TypePtr> &types, const std::string_view &delim);

        ParamPtr ParseParam();
        bool ParseParamList(std::vector<ParamPtr> &params, const std::string_view &delim);
        void ParseParamMap(std::map<std::string, ParamPtr> &params, const std::string_view &delim);

        void ParseTypeAlias();

        StmtPtr ParseImportStmt();
        ImportMapping ParseImportMapping();

        StmtPtr ParseStmt();
        StmtPtr ParseVariableStmt();
        StmtPtr ParseForStmt();
        StmtPtr ParseFunctionStmt();
        StmtPtr ParseIfStmt();
        StmtPtr ParseReturnStmt();
        StmtPtr ParseScopeStmt();
        StmtPtr ParseSwitchStmt();

        ExprPtr ParseExpr();
        ExprPtr ParseBinaryExpr(ExprPtr lhs, unsigned min_pre);
        ExprPtr ParseFormatExpr();
        ExprPtr ParseFunctionExpr();
        ExprPtr ParseOperandExpr();
        ExprPtr ParsePrimaryExpr();
        ExprPtr ParseScopeExpr();
        ExprPtr ParseStructExpr();
        ExprPtr ParseSwitchExpr();
        ExprPtr ParseTupleExpr();

        TypeContext &m_TypeContext;
        TemplateContext &m_TemplateContext;

        std::istream &m_Stream;
        std::map<std::string, Macro> &m_MacroMap;
        bool m_IsImport;
        std::set<std::filesystem::path> m_ParsedSet;

        int m_C;
        SourceLocation m_Where;
        Token m_Token;

        bool m_IsTemplate = false;
        SourceLocation m_TemplateWhere;
        std::stringstream m_TemplateBuffer;
    };
}
