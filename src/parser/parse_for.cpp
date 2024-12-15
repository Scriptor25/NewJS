#include <NJS/AST.hpp>
#include <NJS/Error.hpp>
#include <NJS/Param.hpp>
#include <NJS/Parser.hpp>
#include <NJS/Type.hpp>

NJS::StmtPtr NJS::Parser::ParseFor()
{
    StmtPtr init, loop;
    ExprPtr condition;

    const auto where = Expect("for").Where;

    Expect("(");
    StackPush();
    if (!NextAt(";"))
    {
        init = ParseStmt();
        if (const auto of = NextAt("of"); of || NextAt("in"))
        {
            const auto var_init = std::dynamic_pointer_cast<VariableStmt>(init);
            const auto value = ParseExpression();
            Expect(")");

            var_init->Name->CreateVars(*this, value->Type->GetElement());

            const auto body = ParseStmt();

            StackPop();
            return std::make_shared<ForInOfStmt>(where, *var_init, of, value, body);
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
        loop = ParseStmt();
        Expect(")");
    }

    const auto body = ParseStmt();

    StackPop();
    return std::make_shared<ForStmt>(where, init, condition, loop, body);
}
