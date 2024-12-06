#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>
#include <NJS/Type.hpp>

NJS::ExprPtr NJS::Parser::ParseOperand()
{
    auto ptr = ParsePrimary();
    do
    {
        if (NextAt("."))
        {
            const auto member = Expect(TokenType_Symbol).StringValue;
            ptr = std::make_shared<MemberExpr>(ptr, member);
            continue;
        }

        if (NextAt("("))
        {
            std::vector<ExprPtr> args;
            while (!NextAt(")"))
            {
                args.push_back(ParseExpression());
                if (!At(")"))
                    Expect(",");
            }
            ptr = std::make_shared<CallExpr>(ptr, args);
            continue;
        }

        if (NextAt("["))
        {
            const auto index = ParseExpression();
            Expect("]");
            ptr = std::make_shared<SubscriptExpr>(ptr, index);
            continue;
        }

        if (At("++") || At("--"))
        {
            const auto op = Skip().StringValue;
            ptr = std::make_shared<UnaryExpr>(op, true, ptr);
            continue;
        }

        return ptr;
    }
    while (true);
}
