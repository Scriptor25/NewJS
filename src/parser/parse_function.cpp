#include <NJS/AST.hpp>
#include <NJS/Context.hpp>
#include <NJS/Param.hpp>
#include <NJS/Parser.hpp>

NJS::FunctionStmtPtr NJS::Parser::ParseFunction()
{
    Expect("function");
    const auto name = Expect(TokenType_Symbol).StringValue;

    std::vector<ParamPtr> params;
    Expect("(");
    const auto vararg = ParseParamList(params, ")");

    TypePtr result_type;
    if (NextAt(":"))
        result_type = ParseType();
    else result_type = m_Ctx.GetVoidType();

    ScopeStmtPtr body;
    if (At("{")) body = ParseScope();

    return std::make_shared<FunctionStmt>(name, params, vararg, result_type, body);
}
