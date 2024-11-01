#include <NJS/Token.hpp>

std::ostream& NJS::operator<<(std::ostream& os, const TokenType type)
{
    return os << std::format("{}", type);
}

std::ostream& NJS::operator<<(std::ostream& os, const Token& token)
{
    return os << std::format("{}", token);
}
