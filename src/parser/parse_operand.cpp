#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>
#include <NJS/Type.hpp>

NJS::ExprPtr NJS::Parser::ParseOperand()
{
    auto ptr = ParsePrimary();
    do
    {
        const auto where = m_Token.Where;

        if (NextAt("."))
        {
            const auto name = Expect(TokenType_Symbol).StringValue;
            const auto [type_, index_] = ptr->Type->Member(name);
            ptr = std::make_shared<MemberExpr>(where, type_, ptr, name);
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
            ptr = std::make_shared<CallExpr>(where, type, ptr, args);
            continue;
        }

        if (NextAt("["))
        {
            const auto index = ParseExpression();
            Expect("]");
            const auto type = ptr->Type->Element();
            ptr = std::make_shared<SubscriptExpr>(where, type, ptr, index);
            continue;
        }

        if (At("++") || At("--"))
        {
            const auto op = Skip().StringValue;
            const auto type = ptr->Type;
            ptr = std::make_shared<UnaryExpr>(where, type, op, true, ptr);
            continue;
        }

        return ptr;
    }
    while (true);
}
