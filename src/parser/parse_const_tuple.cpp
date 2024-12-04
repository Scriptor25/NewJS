#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>

NJS::ExprPtr NJS::Parser::ParseConstTuple()
{
    Expect("[");

    std::vector<ExprPtr> entries;
    while (!At("]") && !AtEof())
    {
        const auto value = ParseExpression();
        entries.push_back(value);

        if (!At("]"))
            Expect(",");
        else NextAt(",");
    }
    Expect("]");

    return std::make_shared<ConstTupleExpr>(entries);
}
