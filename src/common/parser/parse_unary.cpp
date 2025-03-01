#include <newjs/ast.hpp>
#include <newjs/parser.hpp>

NJS::ExpressionPtr NJS::Parser::ParseUnaryExpression()
{
    auto ptr = ParseOperandExpression();

    if (At("++") || At("--"))
    {
        const auto token = Skip();
        ptr = std::make_shared<UnaryExpression>(token.Where, token.String, false, ptr);
    }

    return ptr;
}
