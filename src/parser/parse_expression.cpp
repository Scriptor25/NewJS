#include <NJS/Parser.hpp>

NJS::ExpressionPtr NJS::Parser::ParseExpression()
{
    return ParseBinaryExpression(ParseOperandExpression(), 0);
}
