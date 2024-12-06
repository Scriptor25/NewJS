#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>

NJS::ExprPtr NJS::Parser::ParseBinary(ExprPtr lhs, const unsigned min_pre)
{
    static const std::map<std::string, unsigned> OPERATORS
    {
        {"=", 0},
        {"+=", 0},
        {"-=", 0},
        {"*=", 0},
        {"/=", 0},
        {"%=", 0},
        {"&=", 0},
        {"|=", 0},
        {"^=", 0},
        {"<<=", 0},
        {">>=", 0},
        {"?", 0},
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
    };

    auto get_pre = [&]
    {
        return OPERATORS.at(m_Token.StringValue);
    };
    auto has_pre = [&]
    {
        return OPERATORS.contains(m_Token.StringValue);
    };

    while (At(TokenType_Operator) && has_pre() && get_pre() >= min_pre)
    {
        const auto op_pre = get_pre();
        const auto op = Skip().StringValue;

        auto rhs = ParseOperand();
        while (At(TokenType_Operator) && has_pre() && (get_pre() > op_pre || (!get_pre() && get_pre() >= op_pre)))
            rhs = ParseBinary(rhs, op_pre + (get_pre() > op_pre ? 1 : 0));

        if (op == "?")
        {
            Expect(":");
            const auto else_ = ParseExpression();
            lhs = std::make_shared<TernaryExpr>(lhs, rhs, else_);
        }
        else lhs = std::make_shared<BinaryExpr>(op, lhs, rhs);
    }

    return lhs;
}
