#include <newjs/ast.hpp>
#include <newjs/parser.hpp>

NJS::ExpressionPtr NJS::Parser::ParseOperandExpression()
{
    auto ptr = ParsePrimaryExpression();
    do
    {
        auto where = CurrentLocation();

        if (NextAt("."))
        {
            const auto name = Expect(TokenType_Symbol).String;
            ptr = std::make_shared<MemberExpression>(where, ptr, name);
            continue;
        }

        if (NextAt("("))
        {
            std::vector<ExpressionPtr> arguments;
            while (!NextAt(")"))
            {
                arguments.emplace_back(ParseExpression());
                if (!At(")"))
                    Expect(",");
            }
            ptr = std::make_shared<CallExpression>(where, ptr, arguments);
            continue;
        }

        if (NextAt("["))
        {
            const auto index = ParseExpression();
            Expect("]");
            ptr = std::make_shared<SubscriptExpression>(where, ptr, index);
            continue;
        }

        if (NextAt("as"))
        {
            const auto type = ParseType();
            ptr = std::make_shared<CastExpression>(where, type, ptr);
            continue;
        }

        return ptr;
    }
    while (true);
}
