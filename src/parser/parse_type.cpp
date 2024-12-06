#include <NJS/Context.hpp>
#include <NJS/Error.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Parser.hpp>

NJS::TypePtr NJS::Parser::ParseType()
{
    TypePtr type;

    const auto where = m_Token.Where;

    if (At("[")) type = ParseTupleType();
    else if (At("{")) type = ParseObjectType();
    else if (At("(")) type = ParseFunctionType();
    else type = m_Ctx.GetType(Expect(TokenType_Symbol).StringValue);

    if (!type)
        Error(where, "expected a valid typename here");

    while (NextAt("["))
    {
        if (At(TokenType_Number))
        {
            const auto count = Skip().NumberValue;
            type = m_Ctx.GetArrayType(type, count);
        }
        else type = m_Ctx.GetVectorType(type);
        Expect("]");
    }

    return type;
}

NJS::TypePtr NJS::Parser::ParseTupleType()
{
    Expect("[");
    std::vector<TypePtr> types;
    ParseTypeList(types, "]");
    return m_Ctx.GetTupleType(types);
}

NJS::TypePtr NJS::Parser::ParseObjectType()
{
    Expect("{");
    std::map<std::string, TypePtr> types;
    ParseTypeMap(types, "}");
    return m_Ctx.GetObjectType(types);
}

NJS::TypePtr NJS::Parser::ParseFunctionType()
{
    Expect("(");
    std::vector<TypePtr> params;
    const auto vararg = ParseTypeList(params, ")");
    TypePtr result;
    if (NextAt(":"))
        result = ParseType();
    else result = m_Ctx.GetVoidType();
    return m_Ctx.GetFunctionType(params, result, vararg);
}

bool NJS::Parser::ParseTypeList(std::vector<TypePtr>& types, const std::string& delim)
{
    while (!NextAt(delim))
    {
        if (NextAt("."))
        {
            Expect(".");
            Expect(".");
            Expect(delim);
            return true;
        }
        types.push_back(ParseType());
        if (!At(delim))
            Expect(",");
        else NextAt(",");
    }
    return false;
}

void NJS::Parser::ParseTypeMap(std::map<std::string, TypePtr>& types, const std::string& delim)
{
    while (!NextAt(delim))
    {
        const auto name = Expect(TokenType_Symbol).StringValue;
        Expect(":");
        types[name] = ParseType();

        if (!At(delim))
            Expect(",");
        else NextAt(",");
    }
}
