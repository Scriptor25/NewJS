#include <NJS/Context.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Parser.hpp>

NJS::TypePtr NJS::Parser::ParseType()
{
    std::vector<TypePtr> types;
    do
    {
        TypePtr type;

        if (NextAt("void")) type = m_Ctx.GetVoidType();
        else if (NextAt("boolean")) type = m_Ctx.GetBooleanType();
        else if (NextAt("number")) type = m_Ctx.GetNumberType();
        else if (NextAt("string")) type = m_Ctx.GetStringType();
        else if (At("[")) type = ParseTupleType();
        else if (At("{")) type = ParseObjectType();
        else
        {
            const auto where = m_Token.Where;
            type = m_Ctx.GetType(Expect(TokenType_Symbol).StringValue);
            if (!type) Error(where, "expected some kind of type here");
        }

        while (NextAt("["))
        {
            Expect("]");
            type = m_Ctx.GetArrayType(type);
        }
        types.push_back(type);
    }
    while (NextAt("|"));

    if (types.size() == 1) return types.front();
    return m_Ctx.GetMultiType(types);
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

void NJS::Parser::ParseTypeList(std::vector<TypePtr>& types, const std::string& delim)
{
    while (!NextAt(delim))
    {
        types.push_back(ParseType());
        if (!At(delim))
            Expect(",");
        else NextAt(",");
    }
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
