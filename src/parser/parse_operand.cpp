#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>
#include <NJS/Type.hpp>

NJS::ExpressionPtr NJS::Parser::ParseOperandExpression()
{
    auto ptr = ParsePrimaryExpression();
    do
    {
        const auto where = m_Token.Where;

        if (NextAt("."))
        {
            const auto name = Expect(TokenType_Symbol).StringValue;
            ptr = std::make_shared<MemberExpression>(where, ptr, name);
            continue;
        }

        if (NextAt("("))
        {
            std::vector<ExpressionPtr> args;
            while (!NextAt(")"))
            {
                args.push_back(ParseExpression());
                if (!At(")"))
                    Expect(",");
            }
            ptr = std::make_shared<CallExpression>(where, ptr, args);
            continue;
        }

        if (NextAt("["))
        {
            const auto index = ParseExpression();
            Expect("]");
            ptr = std::make_shared<SubscriptExpression>(where, ptr, index);
            continue;
        }

        if (At("++") || At("--"))
        {
            const auto op = Skip().StringValue;
            ptr = std::make_shared<UnaryExpression>(where, op, true, ptr);
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
