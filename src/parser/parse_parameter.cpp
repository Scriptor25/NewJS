#include <NJS/Parameter.hpp>
#include <NJS/Parser.hpp>

NJS::ParameterPtr NJS::Parser::ParseParameter()
{
    ParameterPtr parameter;
    if (NextAt("{"))
    {
        std::map<std::string, ParameterPtr> parameters;
        ParseParameterMap(parameters, "}");
        parameter = std::make_shared<DestructureObject>(parameters);
    }
    else if (NextAt("["))
    {
        std::vector<ParameterPtr> parameters;
        ParseParameterList(parameters, "]");
        parameter = std::make_shared<DestructureArray>(parameters);
    }
    else
    {
        auto name = Expect(TokenType_Symbol).StringValue;
        parameter = std::make_shared<Parameter>(name);
    }

    if (NextAt(":"))
        parameter->Type = ParseType();

    return parameter;
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
        else
            NextAt(",");
    }
    Expect(delimiter);
    return false;
}

void NJS::Parser::ParseParameterMap(std::map<std::string, ParameterPtr> &parameters, const std::string &delimiter)
{
    while (!At(delimiter) && !AtEof())
    {
        auto name = Expect(TokenType_Symbol).StringValue;
        if (NextAt(":"))
            parameters[name] = ParseParameter();
        else
            parameters[name] = std::make_shared<Parameter>(name);

        if (!At(delimiter))
            Expect(",");
        else
            NextAt(",");
    }
    Expect(delimiter);
}
