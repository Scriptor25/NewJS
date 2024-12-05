#include <sstream>
#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>

NJS::ExprPtr NJS::Parser::ParseFormat()
{
    auto where = Expect("$").Where;
    auto source = Expect(TokenType_String).StringValue;

    std::map<size_t, std::string> statics;
    std::map<size_t, ExprPtr> dynamics;

    size_t index = 0;
    for (size_t beg; (beg = source.find('{')) != std::string::npos;)
    {
        if (source[beg + 1] == '{')
        {
            const auto str = source.substr(0, beg + 1);
            statics[index++] = str;
            source.erase(0, beg + 2);
            continue;
        }
        if (const auto str = source.substr(0, beg); !str.empty()) statics[index++] = str;
        source.erase(0, beg + 1);

        std::stringstream stream(source);
        Parser parser(m_Ctx, stream, "<dynamic>");
        dynamics[index++] = parser.ParseExpression();

        source.erase(0, static_cast<size_t>(stream.tellg()) - 1);
    }
    if (!source.empty()) statics[index++] = source;

    return std::make_shared<FormatExpr>(index, statics, dynamics);
}