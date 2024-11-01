#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>

NJS::ExprPtr NJS::Parser::ParseOperand()
{
    auto ptr = ParsePrimary();
    do
    {
        if (NextAt("."))
        {
            const auto member = Expect(TokenType_Symbol).StringValue;
            const auto type = ptr->Type->Member(member);
            ptr = std::make_shared<MemberExpr>(type, ptr, member);
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
            const auto type = ptr->Type->Result();
            ptr = std::make_shared<CallExpr>(type, ptr, args);
            continue;
        }

        if (NextAt("["))
        {
            const auto index = ParseExpression();
            Expect("]");
            const auto type = ptr->Type->Element();
            ptr = std::make_shared<SubscriptExpr>(type, ptr, index);
            continue;
        }

        return ptr;
    }
    while (true);
}
