#include <sstream>
#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TypeContext.hpp>

NJS::ExpressionPtr NJS::Parser::ParseFormatExpression()
{
    const auto where = Expect("$").Where;
    auto source = Expect(TokenType_String).StringValue;

    std::map<unsigned, std::string> statics;
    std::map<unsigned, ExpressionPtr> dynamics;

    unsigned index = 0;
    for (size_t beg; (beg = source.find('{')) != std::string::npos;)
    {
        if (source[beg + 1] == '{')
        {
            const auto str = source.substr(0, beg + 1);
            statics[index++] = str;
            source.erase(0, beg + 2);
            continue;
        }
        if (const auto str = source.substr(0, beg); !str.empty())
            statics[index++] = str;
        source.erase(0, beg + 1);

        std::stringstream stream(source);
        Parser parser(m_TypeContext, m_TemplateContext, stream, SourceLocation("<dynamic>"), m_MacroMap);
        dynamics[index++] = parser.ParseExpression();

        source.erase(0, static_cast<unsigned>(stream.tellg()) - 1);
    }
    if (!source.empty())
        statics[index++] = source;

    return std::make_shared<FormatExpression>(where, index, statics, dynamics);
}
