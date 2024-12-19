#include <NJS/Parser.hpp>

NJS::ExprPtr NJS::Parser::ParseExpr()
{
    return ParseBinaryExpr(ParseOperandExpr(), 0);
}
