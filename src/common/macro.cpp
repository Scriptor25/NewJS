#include <sstream>
#include <newjs/macro.hpp>
#include <newjs/newjs.hpp>
#include <newjs/parser.hpp>

static std::string to_upper(const std::string &src)
{
    auto string = src;
    for (auto &c: string)
        c = static_cast<char>(std::toupper(c));
    return string;
}

static void replace_all(std::string &src, const std::string &find, const std::string &replace)
{
    for (size_t pos; (pos = src.find(find)) != std::string::npos;)
        src.replace(pos, find.size(), replace);
}

NJS::ExpressionPtr NJS::Macro::Inflate(Parser &parent) const
{
    auto source = Source;
    if (!Parameters.empty())
    {
        parent.Expect("(");
        for (const auto &parameter: Parameters)
        {
            std::string argument;
            while (!parent.At(",") && !parent.At(")") && !parent.AtEof())
                argument += parent.Skip().StringValue + ' ';
            argument.pop_back();

            replace_all(source, "##" + parameter, to_upper(argument));
            replace_all(source, "#" + parameter, argument);

            if (!parent.At(")"))
                parent.Expect(",");
        }
        parent.Expect(")");
    }

    std::stringstream stream(source);
    Parser parser(
        parent.GetTypeContext(),
        parent.GetTemplateContext(),
        stream,
        SourceLocation("<macro>"),
        parent.GetMacroMap(),
        parent.IsMain());
    return parser.ParseExpression();
}
