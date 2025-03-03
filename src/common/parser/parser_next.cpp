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
        {"*", {'*', '='}},
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
    std::string value;
    auto is_float = false;

    auto c = Get();
    while (c >= 0 || state != State_Idle)
    {
        switch (state)
        {
            case State_Idle:
                switch (c)
                {
                    case '\n':
                        NewLine();
                        c = Get();
                        break;

                    case '.':
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
                        value += static_cast<char>(c);
                        state = State_Operator;
                        c = Get();
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
                        value += static_cast<char>(c);
                        return m_Token = {where, TokenType_Other, value, value};

                    case '0':
                        where = m_Where;
                        c = Get();
                        if (c == 'b' || c == 'B')
                        {
                            state = State_Bin;
                            c = Get();
                            break;
                        }
                        if (c == 'x' || c == 'X')
                        {
                            state = State_Hex;
                            c = Get();
                            break;
                        }
                        if ('0' <= c && c <= '7')
                        {
                            value += '0';
                            state = State_Oct;
                            break;
                        }

                        value += '0';
                        state = State_Dec;
                        continue;

                    case '"':
                        where = m_Where;
                        state = State_String;
                        c = Get();
                        break;

                    case '\'':
                        where = m_Where;
                        state = State_Char;
                        c = Get();
                        break;

                    default:
                        where = m_Where;
                        if (isdigit(c))
                        {
                            state = State_Dec;
                            break;
                        }
                        if (isalpha(c) || c == '_')
                        {
                            state = State_Symbol;
                            break;
                        }
                        c = Get();
                        break;
                }
                break;

            case State_Comment_Line:
                if (c == '\n')
                {
                    state = State_Idle;
                    NewLine();
                }
                c = Get();
                break;

            case State_Comment_Block:
                if (c == '*')
                {
                    c = Get();
                    if (c == '/')
                        state = State_Idle;
                    else if (c == '\n')
                        NewLine();
                }
                else if (c == '\n')
                    NewLine();
                c = Get();
                break;

            case State_Operator:
                if (value == "/" && c == '/')
                {
                    state = State_Comment_Line;
                    value.clear();
                    c = Get();
                    break;
                }
                if (value == "/" && c == '*')
                {
                    state = State_Comment_Block;
                    value.clear();
                    c = Get();
                    break;
                }
                if (operator_append.contains(value) && operator_append.at(value).contains(c))
                {
                    value += static_cast<char>(c);
                    c = Get();
                    break;
                }
                UnGet();
                return m_Token = {where, TokenType_Operator, value, value};

            case State_Bin:
                if ('0' > c || c > '1')
                {
                    UnGet();
                    return m_Token = {where, TokenType_Int, "0b" + value, value, (std::stoull(value, nullptr, 2))};
                }
                value += static_cast<char>(c);
                c = Get();
                break;

            case State_Oct:
                if ('0' > c || c > '7')
                {
                    UnGet();
                    return m_Token = {where, TokenType_Int, "0" + value, value, (std::stoull(value, nullptr, 8))};
                }
                value += static_cast<char>(c);
                c = Get();
                break;

            case State_Dec:
                if (isdigit(c))
                {
                    value += static_cast<char>(c);
                    c = Get();
                    break;
                }
                if (c == 'e' || c == 'E')
                {
                    if (is_float)
                        Error(where, "token is marked as floating point multiple times");

                    is_float = true;
                    value += static_cast<char>(c);
                    c = Get();
                    if (c == '+' || c == '-')
                    {
                        value += static_cast<char>(c);
                        c = Get();
                    }
                    break;
                }
                if (c == '.')
                {
                    if (is_float)
                        Error(where, "token is marked as floating point multiple times");

                    is_float = true;
                    value += static_cast<char>(c);
                    c = Get();
                    break;
                }
                UnGet();
                if (is_float)
                    return m_Token = {where, TokenType_FP, value, value, 0, std::stod(value)};
                return m_Token = {where, TokenType_Int, value, value, std::stoull(value, nullptr, 10)};

            case State_Hex:
                if (!isxdigit(c))
                {
                    UnGet();
                    return m_Token = {where, TokenType_Int, "0x" + value, value, (std::stoull(value, nullptr, 16))};
                }
                value += static_cast<char>(c);
                c = Get();
                break;

            case State_Symbol:
                if (!(isalnum(c) || c == '_'))
                {
                    UnGet();
                    return m_Token = {where, TokenType_Symbol, value, value};
                }
                value += static_cast<char>(c);
                c = Get();
                break;

            case State_String:
                if (c == '"')
                    return m_Token = {where, TokenType_String, '"' + value + '"', value};
                if (c == '\\')
                    c = Escape(Get());
                value += static_cast<char>(c);
                c = Get();
                break;

            case State_Char:
                if (c == '\'')
                    return m_Token = {where, TokenType_Char, '\'' + value + '\'', value};
                if (c == '\\')
                    c = Escape(Get());
                value += static_cast<char>(c);
                c = Get();
                break;
        }
    }

    return m_Token = {m_Where};
}
