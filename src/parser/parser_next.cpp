#include <istream>
#include <map>
#include <NJS/Parser.hpp>

NJS::Token& NJS::Parser::Next()
{
    enum State
    {
        State_Bin,
        State_Comment_Block,
        State_Comment_Line,
        State_Dec,
        State_Hex,
        State_Idle,
        State_Oct,
        State_Operator,
        State_String,
        State_Char,
        State_Symbol,
    };

    static std::map<int, bool> comp_op
    {
        {'+', true},
        {'-', true},
        {'*', true},
        {'&', true},
        {'|', true},
        {'^', true},
        {'<', true},
        {'>', true},
        {'=', true},
    };

    State state = State_Idle;
    SourceLocation where;
    std::string value;
    bool is_float = false;

    while (m_C >= 0 || state != State_Idle)
    {
        switch (state)
        {
        case State_Idle:
            switch (m_C)
            {
            case '/':
                where = m_Where;
                Get();
                if (m_C == '/')
                {
                    state = State_Comment_Line;
                    break;
                }
                if (m_C == '*')
                {
                    state = State_Comment_Block;
                    break;
                }
                value += '/';
                state = State_Operator;
                continue;

            case '\n':
                NewLine();
                break;

            case '?':
            case '!':
            case '~':
            case '+':
            case '-':
            case '*':
            case '%':
            case '&':
            case '|':
            case '^':
            case '<':
            case '>':
            case '=':
            case '$':
                where = m_Where;
                value += static_cast<char>(m_C);
                state = State_Operator;
                break;

            case '(':
            case ')':
            case '{':
            case '}':
            case '[':
            case ']':
            case '@':
            case '#':
            case ',':
            case '.':
            case ';':
            case ':':
                where = m_Where;
                value += static_cast<char>(m_C);
                Get();
                return m_Token = {where, TokenType_Other, value};

            case '0':
                where = m_Where;
                Get();
                if (m_C == 'b' || m_C == 'B')
                {
                    state = State_Bin;
                    break;
                }
                if (m_C == 'x' || m_C == 'X')
                {
                    state = State_Hex;
                    break;
                }
                if ('0' <= m_C && m_C <= '7')
                {
                    value += '0';
                    value += static_cast<char>(m_C);
                    state = State_Oct;
                    break;
                }

                value += '0';
                state = State_Dec;
                continue;

            case '"':
                where = m_Where;
                state = State_String;
                break;

            case '\'':
                where = m_Where;
                state = State_Char;
                break;

            default:
                where = m_Where;
                if (isdigit(m_C))
                {
                    value += static_cast<char>(m_C);
                    state = State_Dec;
                    break;
                }
                if (isalpha(m_C) || m_C == '_')
                {
                    value += static_cast<char>(m_C);
                    state = State_Symbol;
                    break;
                }
                break;
            }
            break;

        case State_Comment_Line:
            if (m_C == '\n')
            {
                state = State_Idle;
                NewLine();
            }
            break;

        case State_Comment_Block:
            if (m_C == '*')
            {
                Get();
                if (m_C == '/')
                    state = State_Idle;
                else if (m_C == '\n') NewLine();
            }
            else if (m_C == '\n') NewLine();
            break;

        case State_Operator:
            if (!comp_op[m_C])
                return m_Token = {where, TokenType_Operator, value};
            value += static_cast<char>(m_C);
            break;

        case State_Bin:
            if ('0' > m_C || m_C > '1')
                return m_Token = {where, TokenType_Int, value, (std::stoull(value, nullptr, 2))};
            value += static_cast<char>(m_C);
            break;

        case State_Oct:
            if ('0' > m_C || m_C > '7')
                return m_Token = {where, TokenType_Int, value, (std::stoull(value, nullptr, 8))};
            value += static_cast<char>(m_C);
            break;

        case State_Dec:
            if (isdigit(m_C))
            {
                value += static_cast<char>(m_C);
                break;
            }
            if (m_C == '.')
            {
                is_float = true;
                value += static_cast<char>(m_C);
                break;
            }
            if (is_float)
                return m_Token = {where, TokenType_FP, value, 0, std::stod(value)};
            return m_Token = {where, TokenType_Int, value, std::stoull(value, nullptr, 10)};

        case State_Hex:
            if (!isxdigit(m_C))
                return m_Token = {where, TokenType_Int, value, (std::stoull(value, nullptr, 16))};
            value += static_cast<char>(m_C);
            break;

        case State_Symbol:
            if (!(isalnum(m_C) || m_C == '_'))
                return m_Token = {where, TokenType_Symbol, value};
            value += static_cast<char>(m_C);
            break;

        case State_String:
            if (m_C == '"')
            {
                Get();
                return m_Token = {where, TokenType_String, value};
            }
            if (m_C == '\\') Escape();
            value += static_cast<char>(m_C);
            break;

        case State_Char:
            if (m_C == '\'')
            {
                Get();
                return m_Token = {where, TokenType_Char, value};
            }
            if (m_C == '\\') Escape();
            value += static_cast<char>(m_C);
            break;
        }

        Get();
    }

    return m_Token = {};
}
