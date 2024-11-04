#pragma once

#include <functional>
#include <memory>
#include <NJS/Token.hpp>

namespace NJS
{
    class Context;

    typedef std::shared_ptr<struct Type> TypePtr;
    typedef std::shared_ptr<struct Stmt> StmtPtr;
    typedef std::shared_ptr<struct ScopeStmt> ScopeStmtPtr;
    typedef std::shared_ptr<struct FunctionStmt> FunctionStmtPtr;
    typedef std::shared_ptr<struct VariableStmt> VariableStmtPtr;
    typedef std::shared_ptr<struct IfStmt> IfStmtPtr;
    typedef std::shared_ptr<struct ReturnStmt> ReturnStmtPtr;
    typedef std::shared_ptr<struct Expr> ExprPtr;
    typedef std::shared_ptr<struct Param> ParamPtr;
    typedef std::function<void(const StmtPtr&)> Callback;

    class Parser
    {
    public:
        Parser(Context&, std::istream&, std::string);

        void Parse(const Callback&);

    private:
        int Get();
        void NewLine();
        void Escape();
        Token& Next();

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
        void ParseTypeList(std::vector<TypePtr>&, const std::string&);
        void ParseTypeMap(std::map<std::string, TypePtr>&, const std::string&);

        ParamPtr ParseParam();
        bool ParseParamList(std::vector<ParamPtr>&, const std::string&);
        void ParseParamMap(std::map<std::string, ParamPtr>&, const std::string&);

        void ParseTypeAlias();
        void ParseInterface();

        StmtPtr ParseLine();
        ScopeStmtPtr ParseScope();
        FunctionStmtPtr ParseFunction();
        VariableStmtPtr ParseVariable();
        IfStmtPtr ParseIf();
        StmtPtr ParseFor();
        ReturnStmtPtr ParseReturn();

        ExprPtr ParseExpression();
        ExprPtr ParseBinary(ExprPtr lhs, int min_pre);
        ExprPtr ParseOperand();
        ExprPtr ParsePrimary();

        Context& m_Ctx;
        std::istream& m_Stream;
        int m_C;
        SourceLocation m_Where{"", 1, 1};
        Token m_Token;
    };
}
