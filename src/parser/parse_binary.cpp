#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>

NJS::ExprPtr NJS::Parser::ParseBinary(ExprPtr lhs, const unsigned min_pre)
{
    std::map<std::string, unsigned> OPERATORS
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
        return OPERATORS[m_Token.StringValue];
    };

    while (At(TokenType_Operator) && get_pre() >= min_pre)
    {
        const auto op = Skip().StringValue;
        const auto op_pre = OPERATORS[op];

        auto rhs = ParseOperand();
        while (At(TokenType_Operator) && (get_pre() > op_pre || (!get_pre() && get_pre() >= op_pre)))
            rhs = ParseBinary(rhs, op_pre + (get_pre() > op_pre ? 1 : 0));

        lhs = std::make_shared<BinaryExpr>(op, lhs, rhs);
    }

    return lhs;
}
