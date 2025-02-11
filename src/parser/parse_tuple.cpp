#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TypeContext.hpp>

NJS::ExprPtr NJS::Parser::ParseTupleExpr()
{
    const auto where = Expect("[").Where;

    std::vector<ExprPtr> elements;
    while (!At("]") && !AtEof())
    {
        const auto value = ParseExpr();
        elements.push_back(value);

        if (!At("]"))
            Expect(",");
        else
            NextAt(",");
    }
    Expect("]");

    return std::make_shared<TupleExpr>(where, elements);
}
