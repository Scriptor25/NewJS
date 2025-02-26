#include <newjs/ast.hpp>
#include <newjs/parser.hpp>
#include <newjs/type_context.hpp>

NJS::ExpressionPtr NJS::Parser::ParseStructExpression()
{
    const auto where = Expect("{").Where;

    std::vector<std::pair<std::string, ExpressionPtr>> elements;
    while (!At("}") && !AtEof())
    {
        const auto [where_, type_, name_, int_, fp_] = Expect(TokenType_Symbol);

        ExpressionPtr value;
        if (!NextAt(":"))
            value = std::make_shared<SymbolExpression>(where_, name_);
        else
            value = ParseExpression();
        elements.emplace_back(name_, value);

        if (!At("}"))
            Expect(",");
        else
            NextAt(",");
    }
    Expect("}");

    TypePtr type;
    if (NextAt(":"))
        type = ParseType();

    return std::make_shared<StructExpression>(where, type, elements);
}
