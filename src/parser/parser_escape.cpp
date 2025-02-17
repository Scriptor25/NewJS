#include <NJS/Parser.hpp>

int NJS::Parser::Escape(const int c)
{
    std::string value;

    switch (c)
    {
        case 'a':
            return '\a';
        case 'b':
            return '\b';
        case 'e':
            return '\x1B';
        case 'f':
            return '\f';
        case 'n':
            return '\n';
        case 'r':
            return '\r';
        case 't':
            return '\t';
        case 'v':
            return '\v';
        case 'x':
            value += static_cast<char>(Get());
            value += static_cast<char>(Get());
            return std::stoi(value, nullptr, 16);
        default:
            if ('0' <= c && c <= '7')
            {
                value += static_cast<char>(c);
                value += static_cast<char>(Get());
                value += static_cast<char>(Get());
                return std::stoi(value, nullptr, 8);
            }
            return c;
    }
}
