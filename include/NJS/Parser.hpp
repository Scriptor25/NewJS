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
        TypePtr ParseObjectType();
        TypePtr ParseFunctionType();
        bool ParseTypeList(std::vector<TypePtr>&, const std::string&);
        void ParseTypeMap(std::map<std::string, TypePtr>&, const std::string&);

        ParamPtr ParseParam();
        bool ParseParamList(std::vector<ParamPtr>&, const std::string&);
        void ParseParamMap(std::map<std::string, ParamPtr>&, const std::string&);

        void ParseTypeAlias();
        void ParseInterface();

        ImportStmtPtr ParseImport();
        ImportMapping ParseImportMapping();

        StmtPtr ParseLine();
        ScopeStmtPtr ParseScope();
        FunctionStmtPtr ParseFunction();
        VariableStmtPtr ParseVariable();
        IfStmtPtr ParseIf();
        StmtPtr ParseFor();
        ReturnStmtPtr ParseReturn();

        ExprPtr ParseExpression();
        ExprPtr ParseBinary(ExprPtr lhs, unsigned min_pre);
        ExprPtr ParseOperand();
        ExprPtr ParsePrimary();
        ExprPtr ParseConstObject();
        ExprPtr ParseConstTuple();
        ExprPtr ParseConstFunction();
        ExprPtr ParseFormat();
        ExprPtr ParseSwitchExpr();
        ExprPtr ParseScopeExpr();

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
        std::vector<std::map<std::string, TypePtr>> m_Stack;
    };
}
