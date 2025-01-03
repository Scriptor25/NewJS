#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>

NJS::StmtPtr NJS::Parser::ParseSwitchStmt()
{
    const auto where = Expect("switch").Where;

    Expect("(");

    const auto condition = ParseExpr();

    Expect(")");

    std::map<StmtPtr, std::vector<ExprPtr>> cases;
    StmtPtr default_case;

    Expect("{");
    while (!At("}") && !AtEof())
    {
        if (!default_case && NextAt("default"))
        {
            if (NextAt("->"))
                default_case = ParseStmt();
            else default_case = ParseScopeStmt();
            continue;
        }

        Expect("case");

        std::vector<ExprPtr> case_entries;
        do case_entries.push_back(ParseExpr());
        while (NextAt(","));

        StmtPtr value;
        if (NextAt("->"))
            value = ParseStmt();
        else value = ParseScopeStmt();

        cases[value] = case_entries;
    }
    Expect("}");

    return std::make_shared<SwitchStmt>(where, condition, cases, default_case);
}

NJS::ExprPtr NJS::Parser::ParseSwitchExpr()
{
    const auto where = Expect("switch").Where;

    Expect("(");

    const auto condition = ParseExpr();

    Expect(")");

    std::map<ExprPtr, std::vector<ExprPtr>> cases;
    ExprPtr default_case;

    Expect("{");
    while (!At("}") && !AtEof())
    {
        if (!default_case && NextAt("default"))
        {
            if (NextAt("->"))
                default_case = ParseExpr();
            else default_case = ParseScopeExpr();
            continue;
        }

        Expect("case");

        std::vector<ExprPtr> case_entries;
        do case_entries.push_back(ParseExpr());
        while (NextAt(","));

        ExprPtr value;
        if (NextAt("->"))
            value = ParseExpr();
        else value = ParseScopeExpr();

        cases[value] = case_entries;
    }
    Expect("}");

    return std::make_shared<SwitchExpr>(where, condition, cases, default_case);
}
