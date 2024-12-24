#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>
#include <NJS/Type.hpp>

NJS::ExprPtr NJS::Parser::ParseOperandExpr()
{
    auto ptr = ParsePrimaryExpr();
    do
    {
        const auto where = m_Token.Where;

        if (NextAt("."))
        {
            const auto name = Expect(TokenType_Symbol).StringValue;
            ptr = std::make_shared<MemberExpr>(where, ptr, name);
            continue;
        }

        if (NextAt("("))
        {
            std::vector<ExprPtr> args;
            while (!NextAt(")"))
            {
                args.push_back(ParseExpr());
                if (!At(")"))
                    Expect(",");
            }
            ptr = std::make_shared<CallExpr>(where, ptr, args);
            continue;
        }

        if (NextAt("["))
        {
            const auto index = ParseExpr();
            Expect("]");
            ptr = std::make_shared<SubscriptExpr>(where, ptr, index);
            continue;
        }

        if (At("++") || At("--"))
        {
            const auto op = Skip().StringValue;
            ptr = std::make_shared<UnaryExpr>(where, op, true, ptr);
            continue;
        }

        if (NextAt("as"))
        {
            const auto type = ParseType();
            ptr = std::make_shared<CastExpr>(where, type, ptr);
            continue;
        }

        return ptr;
    }
    while (true);
}
