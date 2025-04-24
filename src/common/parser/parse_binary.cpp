#include <newjs/ast.hpp>
#include <newjs/parser.hpp>

NJS::ExpressionPtr NJS::Parser::ParseBinaryExpression(ExpressionPtr lhs, const unsigned min_pre)
{
    static const std::map<std::string, unsigned> OPS
    {
        {"=", 0},
        {"+=", 0},
        {"-=", 0},
        {"*=", 0},
        {"/=", 0},
        {"%=", 0},
        {"**=", 0},
        {"&=", 0},
        {"|=", 0},
        {"^=", 0},
        {"<<=", 0},
        {">>=", 0},
        {"?", 0},
        {"??", 0},
        {"||", 1},
        {"^^", 2},
        {"&&", 3},
        {"|", 4},
        {"^", 5},
        {"&", 6},
        {"==", 7},
        {"!=", 7},
        {"<", 8},
        {"<=", 8},
        {">", 8},
        {">=", 8},
        {"<<", 9},
        {">>", 9},
        {"+", 10},
        {"-", 10},
        {"*", 11},
        {"/", 11},
        {"%", 11},
        {"**", 12},
    };

    auto get_pre = [&]
    {
        return OPS.at(m_Token.Value);
    };
    auto has_pre = [&]
    {
        return OPS.contains(m_Token.Value);
    };

    while (At(TokenType_Operator) && has_pre() && get_pre() >= min_pre)
    {
        const auto op_pre = get_pre();
        auto [
            where_,
            type_,
            raw_,
            value_,
            int_,
            float_
        ] = Skip();

        auto rhs = ParseUnaryExpression();
        while (At(TokenType_Operator) && has_pre() && (get_pre() > op_pre || (!get_pre() && get_pre() >= op_pre)))
            rhs = ParseBinaryExpression(rhs, op_pre + (get_pre() > op_pre ? 1 : 0));

        if (value_ == "?")
        {
            Expect(":");
            const auto else_ = ParseExpression();
            lhs = std::make_shared<TernaryExpression>(where_, lhs, rhs, else_);
            continue;
        }

        if (value_ == "??")
        {
            lhs = std::make_shared<TernaryExpression>(where_, lhs, lhs, rhs);
            continue;
        }

        lhs = std::make_shared<BinaryExpression>(where_, value_, lhs, rhs);
    }

    return lhs;
}
