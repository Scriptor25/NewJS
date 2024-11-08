#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>

NJS::StmtPtr NJS::Parser::ParseFor()
{
    StmtPtr init, loop;
    ExprPtr condition;

    Expect("for");
    Expect("(");
    if (!NextAt(";"))
    {
        init = ParseLine();
        if (const auto of = NextAt("of"); of || NextAt("in"))
        {
            const auto value = ParseExpression();
            Expect(")");
            auto body = ParseLine();
            return std::make_shared<ForInOfStmt>(init, of, value, body);
        }
        Expect(";");
    }
    if (!NextAt(";"))
    {
        condition = ParseExpression();
        Expect(";");
    }
    if (!NextAt(")"))
    {
        loop = ParseLine();
        Expect(")");
    }

    auto body = ParseLine();
    return std::make_shared<ForStmt>(init, condition, loop, body);
}
