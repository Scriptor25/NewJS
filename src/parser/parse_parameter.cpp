#include <NJS/Parameter.hpp>
#include <NJS/Parser.hpp>

NJS::ParameterPtr NJS::Parser::ParseParameter(const bool is_const, const bool is_reference)
{
    auto where = m_Token.Where;

    if (NextAt("{"))
    {
        std::map<std::string, ParameterPtr> parameters;
        ParseParameterMap(parameters, "}");
        auto type = NextAt(":") ? ParseType() : nullptr;
        return std::make_shared<DestructureStruct>(
            where,
            parameters,
            type,
            ReferenceInfo(type, is_const, is_reference));
    }

    if (NextAt("["))
    {
        std::vector<ParameterPtr> parameters;
        ParseParameterList(parameters, "]");
        auto type = NextAt(":") ? ParseType() : nullptr;
        return std::make_shared<DestructureTuple>(
            where,
            parameters,
            type,
            ReferenceInfo(type, is_const, is_reference));
    }

    auto name = Expect(TokenType_Symbol).StringValue;
    auto type = NextAt(":") ? ParseType() : nullptr;
    return std::make_shared<Parameter>(
        where,
        name,
        type,
        ReferenceInfo(type, is_const, is_reference));
}

bool NJS::Parser::ParseParameterList(std::vector<ParameterPtr> &parameters, const std::string &delimiter)
{
    while (!At(delimiter) && !AtEof())
    {
        if (NextAt("..."))
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
    const std::string &delimiter)
{
    while (!At(delimiter) && !AtEof())
    {
        if (NextAt("..."))
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
        auto where = m_Token.Where;
        auto name = Expect(TokenType_Symbol).StringValue;
        parameters[name] = NextAt(":")
                               ? ParseParameter(false, false)
                               : std::make_shared<Parameter>(
                                   where,
                                   name,
                                   nullptr,
                                   ReferenceInfo(nullptr, false, false));

        if (!At(delimiter))
            Expect(",");
    }
    Expect(delimiter);
}
