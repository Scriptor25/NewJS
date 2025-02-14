#include <NJS/Parser.hpp>

NJS::ExpressionPtr NJS::Parser::ParseExpression()
{
    return ParseBinaryExpression(ParseUnaryExpression(), 0);
}
