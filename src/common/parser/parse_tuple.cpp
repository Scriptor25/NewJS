#include <newjs/ast.hpp>
#include <newjs/parser.hpp>
#include <newjs/type.hpp>

NJS::ExpressionPtr NJS::Parser::ParseTupleExpression()
{
    const auto where = Expect("[").Where;

    std::vector<ExpressionPtr> elements;
    while (!At("]") && !AtEof())
    {
        elements.emplace_back(ParseExpression());

        if (!At("]"))
            Expect(",");
        else
            NextAt(",");
    }
    Expect("]");

    TypePtr type;
    if (NextAt(":"))
    {
        type = ParseType();
        if (!type->IsArray() && !type->IsTuple())
            Error(
                where,
                "invalid type postfix for tuple or array expression, must be tuple- or array-like, but is {}",
                type);
    }

    return std::make_shared<TupleExpression>(where, type, elements);
}
