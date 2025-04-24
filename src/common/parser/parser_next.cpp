#include <newjs/error.hpp>
#include <newjs/parser.hpp>

NJS::Token &NJS::Parser::Next()
{
    static const std::map<std::string, std::set<int>> operator_append
    {
        {".", {'.'}},
        {"..", {'.'}},
        {"?", {'?'}},
        {"+", {'+', '='}},
        {"-", {'-', '=', '>'}},
        {"*", {'*', '=', '.'}},
        {"**", {'='}},
        {"/", {'='}},
        {"%", {'='}},
        {"|", {'|', '='}},
        {"||", {'='}},
        {"^", {'^', '='}},
        {"&", {'&', '='}},
        {"&&", {'='}},
        {"<", {'<', '='}},
        {"<<", {'='}},
        {">", {'>', '='}},
        {">>", {'='}},
        {"=", {'=', '>'}},
        {"!", {'='}},
    };

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

    auto state = State_Idle;
    SourceLocation where;
    std::string raw, value;
    auto is_float = false;
    auto is_format = false;

    while (m_Buf >= 0 || state != State_Idle)
    {
        switch (state)
        {
            case State_Idle:
                switch (m_Buf)
                {
                    case '.':
                        where = m_Where;
                        raw += static_cast<char>(m_Buf);
                        value += static_cast<char>(m_Buf);
                        Get();

                        if (isdigit(m_Buf))
                        {
                            state = State_Dec;
                            is_float = true;
                        }
                        else
                        {
                            state = State_Operator;
                        }

                        break;

                    case '[':
                    case '?':
                    case '!':
                    case '~':
                    case '+':
                    case '-':
                    case '*':
                    case '/':
                    case '%':
                    case '&':
                    case '|':
                    case '^':
                    case '<':
                    case '>':
                    case '=':
                    case '$':
                        where = m_Where;
                        raw += static_cast<char>(m_Buf);
                        value += static_cast<char>(m_Buf);
                        state = State_Operator;
                        Get();
                        break;

                    case '(':
                    case ')':
                    case '{':
                    case '}':
                    case ']':
                    case '@':
                    case '#':
                    case ',':
                    case ';':
                    case ':':
                        where = m_Where;
                        raw += static_cast<char>(m_Buf);
                        value += static_cast<char>(m_Buf);
                        Get();
                        return m_Token = {where, TokenType_Other, raw, value};

                    case '0':
                        where = m_Where;
                        raw += static_cast<char>(m_Buf);
                        Get();

                        switch (m_Buf)
                        {
                            case 'b':
                            case 'B':
                                state = State_Bin;
                                raw += static_cast<char>(m_Buf);
                                Get();
                                break;

                            case 'x':
                            case 'X':
                                state = State_Hex;
                                raw += static_cast<char>(m_Buf);
                                Get();
                                break;

                            case '0':
                            case '1':
                            case '2':
                            case '3':
                            case '4':
                            case '5':
                            case '6':
                            case '7':
                                value += '0';
                                state = State_Oct;
                                break;

                            default:
                                value += '0';
                                state = State_Dec;
                                break;
                        }
                        break;

                    case '`':
                        is_format = true;
                    case '"':
                        where = m_Where;
                        state = State_String;
                        raw += static_cast<char>(m_Buf);
                        Get();
                        break;

                    case '\'':
                        where = m_Where;
                        state = State_Char;
                        raw += static_cast<char>(m_Buf);
                        Get();
                        break;

                    default:
                        where = m_Where;
                        if (isdigit(m_Buf))
                        {
                            state = State_Dec;
                            break;
                        }

                        if (isalpha(m_Buf) || m_Buf == '_')
                        {
                            state = State_Symbol;
                            break;
                        }

                        Get();
                        break;
                }
                break;

            case State_Comment_Line:
                if (m_Buf == '\n')
                {
                    state = State_Idle;
                }

                raw += static_cast<char>(m_Buf);
                Get();
                break;

            case State_Comment_Block:
                if (m_Buf == '*')
                {
                    raw += static_cast<char>(m_Buf);
                    Get();

                    if (m_Buf == '/')
                    {
                        state = State_Idle;
                    }
                }

                raw += static_cast<char>(m_Buf);
                Get();
                break;

            case State_Operator:
                if (value == "/")
                {
                    auto br = true;
                    switch (m_Buf)
                    {
                        case '/':
                            state = State_Comment_Line;
                            raw += static_cast<char>(m_Buf);
                            value.clear();
                            Get();
                            break;

                        case '*':
                            state = State_Comment_Block;
                            raw += static_cast<char>(m_Buf);
                            value.clear();
                            Get();
                            break;

                        default:
                            br = false;
                            break;
                    }

                    if (br)
                    {
                        break;
                    }
                }

                if (operator_append.contains(value) && operator_append.at(value).contains(m_Buf))
                {
                    raw += static_cast<char>(m_Buf);
                    value += static_cast<char>(m_Buf);
                    Get();
                    break;
                }

                return m_Token = {where, TokenType_Operator, raw, value};

            case State_Bin:
                if ('0' > m_Buf || m_Buf > '1')
                {
                    return m_Token = {where, TokenType_Int, raw, value, (std::stoull(value, nullptr, 2))};
                }

                raw += static_cast<char>(m_Buf);
                value += static_cast<char>(m_Buf);
                Get();
                break;

            case State_Oct:
                if ('0' > m_Buf || m_Buf > '7')
                {
                    return m_Token = {where, TokenType_Int, raw, value, (std::stoull(value, nullptr, 8))};
                }

                raw += static_cast<char>(m_Buf);
                value += static_cast<char>(m_Buf);
                Get();
                break;

            case State_Dec:
                switch (m_Buf)
                {
                    case 'e':
                    case 'E':
                        if (is_float)
                        {
                            Error(where, "token is marked as floating point multiple times");
                        }

                        is_float = true;
                        value += static_cast<char>(m_Buf);
                        Get();

                        if (m_Buf == '+' || m_Buf == '-')
                        {
                            value += static_cast<char>(m_Buf);
                            Get();
                        }

                        break;

                    case '.':
                        if (is_float)
                        {
                            Error(where, "token is marked as floating point multiple times");
                        }

                        is_float = true;
                        value += static_cast<char>(m_Buf);
                        Get();
                        break;

                    default:
                        if (isdigit(m_Buf))
                        {
                            raw += static_cast<char>(m_Buf);
                            value += static_cast<char>(m_Buf);
                            Get();
                            break;
                        }

                        if (is_float)
                            return m_Token = {where, TokenType_FP, raw, value, 0, std::stod(value)};

                        return m_Token = {where, TokenType_Int, raw, value, std::stoull(value, nullptr, 10)};
                }
                break;

            case State_Hex:
                if (!isxdigit(m_Buf))
                {
                    return m_Token = {where, TokenType_Int, raw, value, (std::stoull(value, nullptr, 16))};
                }

                raw += static_cast<char>(m_Buf);
                value += static_cast<char>(m_Buf);
                Get();
                break;

            case State_Symbol:
                if (!isalnum(m_Buf) && m_Buf != '_')
                {
                    return m_Token = {where, TokenType_Symbol, raw, value};
                }

                raw += static_cast<char>(m_Buf);
                value += static_cast<char>(m_Buf);
                Get();
                break;

            case State_String:
                if (m_Buf == (is_format ? '`' : '"') || m_Buf < 0)
                {
                    raw += static_cast<char>(m_Buf);
                    Get();
                    return m_Token = {where, is_format ? TokenType_Format : TokenType_String, raw, value};
                }

                if (m_Buf == '\\')
                {
                    Get();
                    Escape();
                }

                raw += static_cast<char>(m_Buf);
                value += static_cast<char>(m_Buf);
                Get();
                break;

            case State_Char:
                if (m_Buf == '\'' || m_Buf < 0)
                {
                    raw += static_cast<char>(m_Buf);
                    Get();
                    return m_Token = {where, TokenType_Char, raw, value};
                }

                if (m_Buf == '\\')
                {
                    Get();
                    Escape();
                }

                raw += static_cast<char>(m_Buf);
                value += static_cast<char>(m_Buf);
                Get();
                break;
        }
    }

    return m_Token = {m_Where};
}
