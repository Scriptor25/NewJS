#include <NJS/Parser.hpp>

void NJS::Parser::Escape()
{
    std::string value;

    m_C = Get();
    switch (m_C)
    {
    case 'a':
        m_C = '\a';
        break;
    case 'b':
        m_C = '\b';
        break;
    case 'e':
        m_C = '\x1B';
        break;
    case 'f':
        m_C = '\f';
        break;
    case 'n':
        m_C = '\n';
        break;
    case 'r':
        m_C = '\r';
        break;
    case 't':
        m_C = '\t';
        break;
    case 'v':
        m_C = '\v';
        break;
    case 'x':
        value += static_cast<char>(m_C = Get());
        value += static_cast<char>(m_C = Get());
        m_C = std::stoi(value, nullptr, 16);
        break;
    default:
        if ('0' <= m_C && m_C <= '7')
        {
            value += static_cast<char>(m_C);
            value += static_cast<char>(m_C = Get());
            value += static_cast<char>(m_C = Get());
            m_C = std::stoi(value, nullptr, 8);
            break;
        }
        break;
    }
}
