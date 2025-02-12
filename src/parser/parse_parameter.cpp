#include <NJS/Parameter.hpp>
#include <NJS/Parser.hpp>

NJS::ParameterPtr NJS::Parser::ParseParam()
{
    ParameterPtr param;
    if (NextAt("{"))
    {
        std::map<std::string, ParameterPtr> params;
        ParseParamMap(params, "}");
        param = std::make_shared<DestructureObject>(params);
    }
    else if (NextAt("["))
    {
        std::vector<ParameterPtr> params;
        ParseParamList(params, "]");
        param = std::make_shared<DestructureArray>(params);
    }
    else
    {
        auto name = Expect(TokenType_Symbol).StringValue;
        param = std::make_shared<Parameter>(name);
    }

    if (!NextAt(":"))
        return param;

    param->Type = ParseType();
    return param;
}

bool NJS::Parser::ParseParamList(std::vector<ParameterPtr> &params, const std::string_view &delim)
{
    while (!At(delim) && !AtEof())
    {
        if (NextAt("."))
        {
            Expect(".");
            Expect(".");
            Expect(delim);
            return true;
        }

        params.push_back(ParseParam());
        if (!At(delim))
            Expect(",");
        else
            NextAt(",");
    }
    Expect(delim);
    return false;
}

void NJS::Parser::ParseParamMap(std::map<std::string, ParameterPtr> &params, const std::string_view &delim)
{
    while (!At(delim) && !AtEof())
    {
        auto name = Expect(TokenType_Symbol).StringValue;
        if (NextAt(":"))
            params[name] = ParseParam();
        else
            params[name] = std::make_shared<Parameter>(name);

        if (!At(delim))
            Expect(",");
        else
            NextAt(",");
    }
    Expect(delim);
}
