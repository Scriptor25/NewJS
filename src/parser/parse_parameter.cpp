#include <NJS/Parameter.hpp>
#include <NJS/Parser.hpp>

NJS::ParameterPtr NJS::Parser::ParseParameter()
{
    auto where = m_Token.Where;

    ParameterPtr parameter;
    if (NextAt("{"))
    {
        std::map<std::string, ParameterPtr> parameters;
        ParseParameterMap(parameters, "}");
        parameter = std::make_shared<DestructureObject>(where, parameters);
    }
    else if (NextAt("["))
    {
        std::vector<ParameterPtr> parameters;
        ParseParameterList(parameters, "]");
        parameter = std::make_shared<DestructureTuple>(where, parameters);
    }
    else
    {
        auto name = Expect(TokenType_Symbol).StringValue;
        parameter = std::make_shared<Parameter>(where, name);
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
        auto [
            where_,
            type_,
            name_,
            int_,
            fp_
        ] = Expect(TokenType_Symbol);
        if (NextAt(":"))
            parameters[name_] = ParseParameter();
        else
            parameters[name_] = std::make_shared<Parameter>(where_, name_);

        if (!At(delimiter))
            Expect(",");
        else
            NextAt(",");
    }
    Expect(delimiter);
}
