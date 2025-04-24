#include <newjs/parser.hpp>

void NJS::Parser::Escape()
{
    std::string value;

    switch (m_Buf)
    {
        case 'a':
            m_Buf = '\a';
            break;
        case 'b':
            m_Buf = '\b';
            break;
        case 'e':
            m_Buf = '\x1B';
            break;
        case 'f':
            m_Buf = '\f';
            break;
        case 'n':
            m_Buf = '\n';
            break;
        case 'r':
            m_Buf = '\r';
            break;
        case 't':
            m_Buf = '\t';
            break;
        case 'v':
            m_Buf = '\v';
            break;

        case 'x':
            Get();
            value += static_cast<char>(m_Buf);
            Get();
            value += static_cast<char>(m_Buf);
            m_Buf = std::stoi(value, nullptr, 16);
            break;

        default:
            if ('0' <= m_Buf && m_Buf <= '7')
            {
                value += static_cast<char>(m_Buf);
                Get();
                value += static_cast<char>(m_Buf);
                Get();
                value += static_cast<char>(m_Buf);
                m_Buf = std::stoi(value, nullptr, 8);
                break;
            }
            break;
    }
}
