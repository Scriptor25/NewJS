#include <NJS/AST.hpp>
#include <NJS/Operator.hpp>
#include <NJS/Parser.hpp>

NJS::ExprPtr NJS::Parser::ParseBinary(ExprPtr lhs, const int min_pre)
{
    auto pre = [&]
    {
        return Operator::Get(m_Token.StringValue).Precedence;
    };

    while (At(TokenType_Operator) && pre() >= min_pre)
    {
        const auto str = Skip().StringValue;
        auto& op = Operator::Get(str);

        auto rhs = ParseOperand();
        while (At(TokenType_Operator) && (pre() > op.Precedence || (!pre() && pre() >= op.Precedence)))
            rhs = ParseBinary(rhs, op.Precedence + (pre() > op.Precedence ? 1 : 0));

        lhs = std::make_shared<BinaryExpr>(op.Result(m_Ctx, lhs->Type, rhs->Type), str, lhs, rhs);
    }

    return lhs;
}
