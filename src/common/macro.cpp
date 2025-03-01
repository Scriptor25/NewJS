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

static std::string escape(std::string src)
{
    for (unsigned i = 0; i < src.size(); ++i)
    {
        if (src[i] == '"')
            src.insert(src.begin() + i++, '\\');
    }
    return src;
}

NJS::ExpressionPtr NJS::Macro::Inflate(Parser &parent) const try
{
    auto source = Source;
    auto where = parent.CurrentLocation();
    if (!Parameters.empty())
    {
        parent.Expect("(");
        for (const auto &parameter: Parameters)
        {
            auto argument = parent.ParseStatement();
            std::stringstream stream;
            argument->Print(stream);
            auto argument_string = stream.str();

            replace_all(source, "##" + parameter, to_upper(escape(argument_string)));
            replace_all(source, "#" + parameter, escape(argument_string));
            replace_all(source, "%" + parameter, argument_string);

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
        where,
        parent.GetMacroMap(),
        parent.IsMain());
    return parser.ParseExpression();
}
catch (const RTError &error)
{
    Error(error, Where, "in macro '{}'", Source);
}
