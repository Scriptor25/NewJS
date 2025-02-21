#include <NJS/Parameter.hpp>
#include <NJS/Parser.hpp>

NJS::ParameterPtr NJS::Parser::ParseParameter()
{
    auto where = m_Token.Where;

    if (NextAt("{"))
    {
        std::map<std::string, ParameterPtr> parameters;
        ParseParameterMap(parameters, "}");
        auto type = NextAt(":") ? ParseType() : nullptr;
        return std::make_shared<DestructureStruct>(where, parameters, type);
    }

    if (NextAt("["))
    {
        std::vector<ParameterPtr> parameters;
        ParseParameterList(parameters, "]");
        auto type = NextAt(":") ? ParseType() : nullptr;
        return std::make_shared<DestructureTuple>(where, parameters, type);
    }

    auto name = Expect(TokenType_Symbol).StringValue;
    auto type = NextAt(":") ? ParseType() : nullptr;
    return std::make_shared<Parameter>(where, name, type);
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

        parameters.push_back(ParseParameter());

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
        const auto [
            where_,
            type_,
            name_,
            int_,
            fp_
        ] = Expect(TokenType_Symbol);

        parameters[name_] = NextAt(":")
                                ? ParseParameter()
                                : std::make_shared<Parameter>(where_, name_, nullptr);

        if (!At(delimiter))
            Expect(",");
    }
    Expect(delimiter);
}
