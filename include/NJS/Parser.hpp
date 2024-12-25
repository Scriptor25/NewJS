#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <set>
#include <NJS/Import.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Token.hpp>

namespace NJS
{
    typedef std::function<void(const StmtPtr&)> Callback;

    struct Macro
    {
        std::string Source;
    };

    class Parser
    {
        friend Param;
        friend ImportMapping;

    public:
        Parser(
            TypeContext&,
            std::istream&,
            std::string,
            std::map<std::string, Macro>&,
            bool = false,
            std::set<std::filesystem::path>  = {});

        void Parse(const Callback&);

    private:
        int Get();
        void NewLine();
        void Escape();
        Token& Next();

        [[nodiscard]] bool AtEof() const;
        [[nodiscard]] bool At(TokenType) const;
        [[nodiscard]] bool At(const std::string&) const;

        bool NextAt(TokenType);
        bool NextAt(const std::string&);

        Token Skip();

        Token Expect(TokenType);
        Token Expect(const std::string&);

        TypePtr ParseType();
        TypePtr ParseTupleType();
        TypePtr ParseStructType();
        TypePtr ParseFunctionType();
        bool ParseTypeList(std::vector<TypePtr>&, const std::string&);
        void ParseTypeMap(std::map<std::string, TypePtr>&, const std::string&);

        ParamPtr ParseParam();
        bool ParseParamList(std::vector<ParamPtr>&, const std::string&);
        void ParseParamMap(std::map<std::string, ParamPtr>&, const std::string&);

        void ParseTypeAlias();

        StmtPtr ParseImportStmt();
        ImportMapping ParseImportMapping();

        StmtPtr ParseStmt();
        StmtPtr ParseDefStmt();
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

        TypeContext& m_Ctx;
        std::istream& m_Stream;
        std::map<std::string, Macro>& m_Macros;
        bool m_Imported;
        std::set<std::filesystem::path> m_Parsed;

        int m_C;
        SourceLocation m_Where{"", 1, 1};
        Token m_Token;
    };
}
