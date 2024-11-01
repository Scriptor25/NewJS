#include <NJS/Context.hpp>
#include <NJS/Param.hpp>
#include <NJS/Parser.hpp>

NJS::ParamPtr NJS::Parser::ParseParam()
{
    ParamPtr param;
    if (NextAt("{"))
    {
        std::map<std::string, ParamPtr> params;
        ParseParamMap(params, "}");
        param = std::make_shared<DestructureObject>(params);
    }
    else if (NextAt("["))
    {
        std::vector<ParamPtr> params;
        ParseParamList(params, "]");
        param = std::make_shared<DestructureArray>(params);
    }
    else
    {
        auto name = Expect(TokenType_Symbol).StringValue;
        param = std::make_shared<Param>(name);
    }

    if (!NextAt(":"))
        return param;

    param->Type = ParseType();
    return param;
}

void NJS::Parser::ParseParamList(std::vector<ParamPtr>& params, const std::string& delim)
{
    while (!NextAt(delim))
    {
        params.push_back(ParseParam());
        if (!At(delim))
            Expect(",");
        else NextAt(",");
    }
}

void NJS::Parser::ParseParamMap(std::map<std::string, ParamPtr>& params, const std::string& delim)
{
    while (!NextAt(delim))
    {
        auto name = Expect(TokenType_Symbol).StringValue;
        if (NextAt(":"))
            params[name] = ParseParam();
        else params[name] = std::make_shared<Param>(name);

        if (!At(delim))
            Expect(",");
        else NextAt(",");
    }
}
