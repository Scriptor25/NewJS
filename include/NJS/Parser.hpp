#pragma once

#include <functional>
#include <memory>
#include <NJS/Import.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Token.hpp>

namespace NJS
{
    typedef std::function<void(const StmtPtr&)> Callback;

    class Parser
    {
        friend Param;
        friend ImportMapping;

    public:
        Parser(
            TypeContext&,
            std::istream&,
            std::string,
            bool = false,
            std::vector<std::map<std::string, TypePtr>>  = {});

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

        void DefOp(const std::string&, const TypePtr&, const TypePtr&, const TypePtr&);

        void StackPush();
        void StackPop();
        TypePtr& DefVar(const std::string&);
        TypePtr GetVar(const std::string&);

        TypeContext& m_Ctx;
        std::istream& m_Stream;

        int m_C;
        SourceLocation m_Where{"", 1, 1};
        Token m_Token;

        bool m_Imported;

        std::map<std::string, std::map<TypePtr, std::map<TypePtr, TypePtr>>> m_BinOps;

        std::vector<std::map<std::string, TypePtr>> m_Stack;
    };
}
