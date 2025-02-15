#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>

NJS::ExpressionPtr NJS::Parser::ParseUnaryExpression()
{
    auto ptr = ParseOperandExpression();

    if (At("++") || At("--"))
    {
        const auto token = Skip();
        ptr = std::make_shared<UnaryExpression>(token.Where, token.StringValue, false, ptr);
    }

    return ptr;
}
