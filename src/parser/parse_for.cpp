#include <NJS/AST.hpp>
#include <NJS/Error.hpp>
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
            const auto var_init = std::dynamic_pointer_cast<VariableStmt>(init);
            if (!var_init)
                Error("for of/in requires a variable initializer");
            const auto value = ParseExpression();
            Expect(")");
            auto body = ParseLine();
            return std::make_shared<ForInOfStmt>(*var_init, of, value, body);
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
