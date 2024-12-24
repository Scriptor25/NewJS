#include <NJS/AST.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Parser.hpp>

NJS::ExprPtr NJS::Parser::ParseStructExpr()
{
    const auto where = Expect("{").Where;

    std::map<std::string, ExprPtr> elements;
    while (!At("}") && !AtEof())
    {
        const auto [where_, type_, name_, int_, fp_] = Expect(TokenType_Symbol);
        if (!NextAt(":"))
        {
            const auto value = std::make_shared<SymbolExpr>(where_, name_);
            elements[name_] = value;
        }
        else
        {
            const auto value = ParseExpr();
            elements[name_] = value;
        }

        if (!At("}"))
            Expect(",");
        else NextAt(",");
    }
    Expect("}");

    return std::make_shared<StructExpr>(where, elements);
}
