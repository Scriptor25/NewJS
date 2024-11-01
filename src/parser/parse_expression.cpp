#include <NJS/Parser.hpp>

NJS::ExprPtr NJS::Parser::ParseExpression()
{
    return ParseBinary(ParseOperand(), 0);
}
