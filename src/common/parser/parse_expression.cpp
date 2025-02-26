#include <newjs/parser.hpp>

NJS::ExpressionPtr NJS::Parser::ParseExpression()
{
    return ParseBinaryExpression(ParseUnaryExpression(), 0);
}
