#include <NJS/AST.hpp>
#include <NJS/Error.hpp>
#include <NJS/Operator.hpp>
#include <NJS/Parser.hpp>

NJS::ExprPtr NJS::Parser::ParseBinaryExpr(ExprPtr lhs, const unsigned min_pre)
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
        const auto [where_, type_, op_, int_, fp_] = Skip();

        auto rhs = ParseOperandExpr();
        while (At(TokenType_Operator) && has_pre() && (get_pre() > op_pre || (!get_pre() && get_pre() >= op_pre)))
            rhs = ParseBinaryExpr(rhs, op_pre + (get_pre() > op_pre ? 1 : 0));

        if (op_ == "?")
        {
            const auto type = rhs->Type;
            Expect(":");
            const auto else_ = ParseExpr();
            lhs = std::make_shared<TernaryExpr>(where_, type, lhs, rhs, else_);
            continue;
        }

        const auto type = OperatorType(m_Ctx, op_, lhs->Type, rhs->Type);
        if (!type)
            Error("undefined binary operator '{} {} {}'", lhs->Type, op_, rhs->Type);
        lhs = std::make_shared<BinaryExpr>(where_, type, op_, lhs, rhs);
    }

    return lhs;
}
