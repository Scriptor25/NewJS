#include <newjs/parameter.hpp>
#include <newjs/parser.hpp>

NJS::ParameterPtr NJS::Parser::ParseParameter(const bool is_const, const bool is_reference)
{
    auto where = CurrentLocation();

    if (NextAt("{"))
    {
        std::map<std::string, ParameterPtr> parameters;

        const auto all = NextAt("*");
        if (all)
            Expect("}");
        else
            ParseParameterMap(parameters, "}");

        auto type = NextAt(":") ? ParseType() : nullptr;

        return std::make_shared<DestructureStruct>(
            where,
            parameters,
            all,
            ReferenceInfo(std::move(type), is_const, is_reference));
    }

    if (NextAt("["))
    {
        std::vector<ParameterPtr> parameters;
        ParseParameterList(parameters, "]", true);
        auto type = NextAt(":") ? ParseType() : nullptr;
        return std::make_shared<DestructureTuple>(
            where,
            parameters,
            ReferenceInfo(std::move(type), is_const, is_reference));
    }

    auto name = Expect(TokenType_Symbol).Value;
    auto type = NextAt(":") ? ParseType() : nullptr;
    return std::make_shared<Parameter>(
        where,
        name,
        ReferenceInfo(std::move(type), is_const, is_reference));
}

bool NJS::Parser::ParseParameterList(
    std::vector<ParameterPtr> &parameters,
    const std::string &delimiter,
    const bool allow_var_arg)
{
    while (!At(delimiter) && !AtEof())
    {
        if (allow_var_arg && NextAt("..."))
        {
            Expect(delimiter);
            return true;
        }

        parameters.emplace_back(ParseParameter(false, false));

        if (!At(delimiter))
            Expect(",");
    }
    Expect(delimiter);
    return false;
}

bool NJS::Parser::ParseReferenceParameterList(
    std::vector<ParameterPtr> &parameters,
    const std::string &delimiter,
    const bool allow_var_arg)
{
    while (!At(delimiter) && !AtEof())
    {
        if (allow_var_arg && NextAt("..."))
        {
            Expect(delimiter);
            return true;
        }

        const auto is_const = NextAt("const");
        const auto is_reference = NextAt("&");
        parameters.emplace_back(ParseParameter(is_const, is_reference));

        if (!At(delimiter))
            Expect(",");
    }
    Expect(delimiter);
    return false;
}

void NJS::Parser::ParseParameterMap(std::map<std::string, ParameterPtr> &parameters, const std::string &delimiter)
{
    while (!At(delimiter) && !AtEof())
    {
        auto where = CurrentLocation();
        auto name = Expect(TokenType_Symbol).Value;
        parameters[name] = NextAt(":")
                               ? ParseParameter(false, false)
                               : std::make_shared<Parameter>(
                                   where,
                                   name,
                                   ReferenceInfo());

        if (!At(delimiter))
            Expect(",");
    }
    Expect(delimiter);
}
