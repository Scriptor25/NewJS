#include <sstream>
#include <newjs/ast.hpp>
#include <newjs/parser.hpp>
#include <newjs/type_context.hpp>

NJS::ExpressionPtr NJS::Parser::ParseFormatExpression()
{
    const auto where = Expect("f").Where;
    if (!At(TokenType_String))
        return ParseSymbolExpression(where, "f");

    const auto source = Expect(TokenType_String).StringValue;

    std::map<unsigned, std::string> statics;
    std::map<unsigned, ExpressionPtr> dynamics;

    unsigned index = 0;
    unsigned current = 0;
    for (size_t beg; (beg = source.find('{', current)) != std::string::npos;)
    {
        if (source[beg + 1] == '{')
        {
            const auto str = source.substr(current, beg + 1);
            statics[index++] = str;
            current = beg + 2;
            continue;
        }
        if (const auto str = source.substr(current, beg); !str.empty())
            statics[index++] = str;
        current = beg + 1;

        std::stringstream stream(source.substr(current));
        Parser parser(
            m_TypeContext,
            m_TemplateContext,
            stream,
            SourceLocation(where.Filename, where.Row, where.Column + current + 1),
            m_MacroMap,
            m_IsMain);
        dynamics[index++] = parser.ParseExpression();

        current += stream.tellg();
    }
    if (current < source.length())
        statics[index++] = source.substr(current);

    return std::make_shared<FormatExpression>(where, index, statics, dynamics);
}
