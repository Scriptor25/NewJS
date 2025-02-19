#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TypeContext.hpp>

NJS::ExpressionPtr NJS::Parser::ParseTupleExpression()
{
    const auto where = Expect("[").Where;

    std::vector<ExpressionPtr> elements;
    while (!At("]") && !AtEof())
    {
        const auto value = ParseExpression();
        elements.push_back(value);

        if (!At("]"))
            Expect(",");
        else
            NextAt(",");
    }
    Expect("]");

    TypePtr type;
    if (NextAt(":"))
        type = ParseType();

    return std::make_shared<TupleExpression>(where, type, elements);
}
