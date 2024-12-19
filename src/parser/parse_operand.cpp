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
            const auto [type_, index_] = ptr->Type->GetMember(name);
            ptr = std::make_shared<MemberExpr>(where, type_, ptr, name);
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
            const auto type = ptr->Type->GetResult();
            ptr = std::make_shared<CallExpr>(where, type, ptr, args);
            continue;
        }

        if (NextAt("["))
        {
            const auto index = ParseExpr();
            Expect("]");
            const auto type = ptr->Type->GetElement();
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
