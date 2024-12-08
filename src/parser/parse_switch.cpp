#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>

NJS::ExprPtr NJS::Parser::ParseSwitchExpr()
{
    StackPush();
    const auto where = Expect("switch").Where;
    Expect("(");
    const auto condition = ParseExpression();
    Expect(")");

    std::map<ExprPtr, std::vector<ExprPtr>> cases;
    ExprPtr default_case;

    Expect("{");
    while (!At("}") && !AtEof())
    {
        if (!default_case && NextAt("default"))
        {
            if (NextAt("->"))
                default_case = ParseExpression();
            else default_case = ParseScopeExpr();
            continue;
        }

        Expect("case");

        std::vector<ExprPtr> case_entries;
        do case_entries.push_back(ParsePrimary());
        while (NextAt(","));

        ExprPtr value;
        if (NextAt("->"))
            value = ParseExpression();
        else value = ParseScopeExpr();

        cases[value] = case_entries;
    }
    Expect("}");
    StackPop();

    const auto type = default_case->Type;
    return std::make_shared<SwitchExpr>(where, type, condition, cases, default_case);
}
