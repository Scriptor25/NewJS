#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TypeContext.hpp>

NJS::ExpressionPtr NJS::Parser::ParseStructExpression()
{
    const auto where = Expect("{").Where;

    std::map<std::string, ExpressionPtr> elements;
    while (!At("}") && !AtEof())
    {
        const auto [where_, type_, name_, int_, fp_] = Expect(TokenType_Symbol);
        if (!NextAt(":"))
        {
            const auto value = std::make_shared<SymbolExpression>(where_, name_);
            elements[name_] = value;
        }
        else
        {
            const auto value = ParseExpression();
            elements[name_] = value;
        }

        if (!At("}"))
            Expect(",");
        else
            NextAt(",");
    }
    Expect("}");

    return std::make_shared<StructExpression>(where, elements);
}
