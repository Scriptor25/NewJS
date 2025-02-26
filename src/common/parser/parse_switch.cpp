#include <newjs/ast.hpp>
#include <newjs/parser.hpp>

NJS::StatementPtr NJS::Parser::ParseSwitchStatement()
{
    const auto where = Expect("switch").Where;

    Expect("(");

    const auto condition = ParseExpression();

    Expect(")");

    std::map<StatementPtr, std::vector<ExpressionPtr>> cases;
    StatementPtr default_case;

    Expect("{");
    while (!At("}") && !AtEof())
    {
        if (!default_case && NextAt("default"))
        {
            if (NextAt("->"))
                default_case = ParseStatement();
            else
                default_case = ParseScopeStatement();
            continue;
        }

        Expect("case");

        std::vector<ExpressionPtr> case_entries;
        do
            case_entries.emplace_back(ParseExpression());
        while (NextAt(","));

        StatementPtr value;
        if (NextAt("->"))
            value = ParseStatement();
        else
            value = ParseScopeStatement();

        cases[value] = case_entries;
    }
    Expect("}");

    return std::make_shared<SwitchStatement>(where, condition, cases, default_case);
}

NJS::ExpressionPtr NJS::Parser::ParseSwitchExpression()
{
    const auto where = Expect("switch").Where;

    Expect("(");

    const auto condition = ParseExpression();

    Expect(")");

    std::map<ExpressionPtr, std::vector<ExpressionPtr>> cases;
    ExpressionPtr default_case;

    Expect("{");
    while (!At("}") && !AtEof())
    {
        if (!default_case && NextAt("default"))
        {
            if (NextAt("->"))
                default_case = ParseExpression();
            else
                default_case = ParseScopeExpression();
            continue;
        }

        Expect("case");

        std::vector<ExpressionPtr> case_entries;
        do
            case_entries.emplace_back(ParseExpression());
        while (NextAt(","));

        ExpressionPtr value;
        if (NextAt("->"))
            value = ParseExpression();
        else
            value = ParseScopeExpression();

        cases[value] = case_entries;
    }
    Expect("}");

    return std::make_shared<SwitchExpression>(where, condition, cases, default_case);
}
