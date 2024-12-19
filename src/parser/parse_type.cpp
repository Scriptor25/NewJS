#include <NJS/Error.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TypeContext.hpp>

NJS::TypePtr NJS::Parser::ParseType()
{
    TypePtr type;

    const auto where = m_Token.Where;

    if (At("[")) type = ParseTupleType();
    else if (At("{")) type = ParseStructType();
    else if (At("(")) type = ParseFunctionType();
    else
    {
        const auto sym = Expect(TokenType_Symbol).StringValue;
        if (sym[0] == 'i') type = m_Ctx.GetIntType(std::stoull(sym.substr(1)), true);
        else if (sym[0] == 'u') type = m_Ctx.GetIntType(std::stoull(sym.substr(1)), false);
        else if (sym[0] == 'f') type = m_Ctx.GetFPType(std::stoull(sym.substr(1)));
        else type = m_Ctx.GetType(sym);
    }

    if (!type)
        Error(where, "expected a valid typename here");

    while (true)
    {
        if (NextAt("["))
        {
            if (At(TokenType_Int))
            {
                const auto count = Skip().IntValue;
                type = m_Ctx.GetArrayType(type, count);
            }
            else type = m_Ctx.GetPointerType(type);
            Expect("]");
            continue;
        }

        if (NextAt("&"))
        {
            type = m_Ctx.GetRefType(type);
            continue;
        }

        break;
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

NJS::TypePtr NJS::Parser::ParseStructType()
{
    Expect("{");
    std::map<std::string, TypePtr> types;
    ParseTypeMap(types, "}");
    return m_Ctx.GetStructType(types);
}

NJS::TypePtr NJS::Parser::ParseFunctionType()
{
    Expect("(");
    std::vector<TypePtr> args;
    const auto vararg = ParseTypeList(args, ")");
    TypePtr result;
    if (NextAt(":"))
        result = ParseType();
    else result = m_Ctx.GetVoidType();
    return m_Ctx.GetFunctionType(result, args, vararg);
}

bool NJS::Parser::ParseTypeList(std::vector<TypePtr>& types, const std::string& delim)
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
        types.push_back(ParseType());
        if (!At(delim))
            Expect(",");
        else NextAt(",");
    }
    Expect(delim);
    return false;
}

void NJS::Parser::ParseTypeMap(std::map<std::string, TypePtr>& types, const std::string& delim)
{
    while (!At(delim) && !AtEof())
    {
        const auto name = Expect(TokenType_Symbol).StringValue;
        Expect(":");
        types[name] = ParseType();

        if (!At(delim))
            Expect(",");
        else NextAt(",");
    }
    Expect(delim);
}
