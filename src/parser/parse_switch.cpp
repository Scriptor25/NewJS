#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>

NJS::ExprPtr NJS::Parser::ParseSwitchExpr()
{
    Expect("switch");
    Expect("(");
    const auto switcher = ParseExpression();
    Expect(")");

    std::vector<std::pair<ExprPtr, ExprPtr>> cases;
    ExprPtr default_case;

    Expect("{");
    while (!At("{") && !AtEof())
    {
        if (!default_case && NextAt("default"))
        {
            if (NextAt("->"))
                default_case = ParseExpression();
            else default_case = ParseScopeExpr();
            continue;
        }

        Expect("case");
        const auto case_entry = ParsePrimary();

        ExprPtr value;
        if (NextAt("->"))
            value = ParseExpression();
        else value = ParseScopeExpr();

        cases.emplace_back(case_entry, value);
    }
    Expect("}");

    return std::make_shared<SwitchExpr>(switcher, cases, default_case);
}
