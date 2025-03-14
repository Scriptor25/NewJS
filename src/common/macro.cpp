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

std::pair<NJS::SourceLocation, std::string> NJS::Macro::PrepareSource(Parser &parent) const
{
    auto source = Source;
    auto where = parent.CurrentLocation();

    if (!Parameters.empty())
    {
        parent.Expect("(");
        for (const auto &[name_, type_]: Parameters)
        {
            std::string argument_string;
            switch (type_)
            {
                case MacroParameterType_Statement:
                {
                    auto argument = parent.ParseStatement();
                    std::stringstream stream;
                    argument->Print(stream);
                    argument_string = stream.str();
                    break;
                }
                case MacroParameterType_Expression:
                {
                    auto argument = parent.ParseExpression();
                    std::stringstream stream;
                    argument->Print(stream);
                    argument_string = stream.str();
                    break;
                }
                case MacroParameterType_Type:
                {
                    auto argument = parent.ParseType();
                    std::stringstream stream;
                    argument->Print(stream);
                    argument_string = stream.str();
                    break;
                }
            }

            replace_all(source, "##" + name_, to_upper(escape(argument_string)));
            replace_all(source, "#" + name_, escape(argument_string));
            replace_all(source, "%" + name_, argument_string);

            if (!parent.At(")"))
                parent.Expect(",");
        }
        parent.Expect(")");
    }

    return {where, source};
}

NJS::ExpressionPtr NJS::Macro::Inflate(Parser &parent) const try
{
    auto [where, source] = PrepareSource(parent);
    std::stringstream stream(source);
    Parser parser(parent, stream, where);
    return parser.ParseExpression();
}
catch (const RTError &error)
{
    Error(error, Where, "failed to inflate macro '{}'", Source);
}

NJS::TypePtr NJS::Macro::InflateType(Parser &parent) const try
{
    auto [where, source] = PrepareSource(parent);
    std::stringstream stream(source);
    Parser parser(parent, stream, where);
    return parser.ParseType();
}
catch (const RTError &error)
{
    Error(error, Where, "failed to inflate type macro '{}'", Source);
}
