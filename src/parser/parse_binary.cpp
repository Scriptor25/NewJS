#include <NJS/AST.hpp>
#include <NJS/Error.hpp>
#include <NJS/Parser.hpp>

NJS::ExprPtr NJS::Parser::ParseBinaryExpr(ExprPtr lhs, const unsigned min_pre)
{
    static const std::map<std::string, unsigned> OPS
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
        {"**", 12},
    };

    auto get_pre = [&]
    {
        return OPS.at(m_Token.StringValue);
    };
    auto has_pre = [&]
    {
        return OPS.contains(m_Token.StringValue);
    };

    while (At(TokenType_Operator) && has_pre() && get_pre() >= min_pre)
    {
        const auto op_pre = get_pre();
        const auto [where_, type_, op_, int_, fp_] = Skip();

        auto rhs = ParseOperandExpr();
        while (At(TokenType_Operator) && has_pre() && (get_pre() > op_pre || (!get_pre() && get_pre() >= op_pre)))
            rhs = ParseBinaryExpr(rhs, op_pre + (get_pre() > op_pre ? 1 : 0));

        if (op_ == "?")
        {
            Expect(":");
            const auto else_ = ParseExpr();
            lhs = std::make_shared<TernaryExpr>(where_, lhs, rhs, else_);
            continue;
        }

        lhs = std::make_shared<BinaryExpr>(where_, op_, lhs, rhs);
    }

    return lhs;
}
