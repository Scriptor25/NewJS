#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>

NJS::ExprPtr NJS::Parser::ParseConstObject()
{
    Expect("{");

    std::map<std::string, ExprPtr> entries;
    while (!At("}") && !AtEof())
    {
        const auto name = Expect(TokenType_Symbol).StringValue;
        if (!NextAt(":"))
            entries[name] = std::make_shared<SymbolExpr>(name);
        else
        {
            const auto value = ParseExpression();
            entries[name] = value;
        }

        if (!At("}"))
            Expect(",");
        else NextAt(",");
    }
    Expect("}");

    return std::make_shared<ConstObjectExpr>(entries);
}
