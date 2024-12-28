#include <NJS/Error.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TemplateContext.hpp>
#include <NJS/TypeContext.hpp>

NJS::TypePtr NJS::Parser::ParseType()
{
    const auto where = m_Token.Where;

    TypePtr type;
    if (At("[")) type = ParseTupleType();
    else if (At("{")) type = ParseStructType();
    else if (At("(")) type = ParseFunctionType();
    else
    {
        const auto sym = Expect(TokenType_Symbol).StringValue;
        if (m_TemplateCtx.HasType(sym))
        {
            std::vector<TypePtr> args;

            Expect("<");
            while (!At(">") && !AtEof())
            {
                args.push_back(ParseType());
                if (!At(">"))
                    Expect(",");
            }
            Expect(">");

            if (!m_IsTemplate)
                type = m_TemplateCtx.InflateType(*this, sym, args);
            else type = m_TypeCtx.GetNoType();
        }
        else
        {
            type = m_TypeCtx.GetType(sym);
            if (!type)
            {
                if (sym == "void") type = m_TypeCtx.GetVoidType();
                else if (sym[0] == 'i') type = m_TypeCtx.GetIntType(std::stoull(sym.substr(1)), true);
                else if (sym[0] == 'u') type = m_TypeCtx.GetIntType(std::stoull(sym.substr(1)), false);
                else if (sym[0] == 'f') type = m_TypeCtx.GetFPType(std::stoull(sym.substr(1)));
            }
        }
    }

    if (!type)
    {
        if (!m_IsTemplate)
            Error(where, "expected a valid typename here");
        type = m_TypeCtx.GetNoType();
    }

    while (true)
    {
        if (NextAt("["))
        {
            if (At(TokenType_Int))
            {
                const auto count = Skip().IntValue;
                type = m_TypeCtx.GetArrayType(type, count);
            }
            else type = m_TypeCtx.GetPointerType(type);
            Expect("]");
            continue;
        }

        if (NextAt("&"))
        {
            type = m_TypeCtx.GetRefType(type);
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
    return m_TypeCtx.GetTupleType(types);
}

NJS::TypePtr NJS::Parser::ParseStructType()
{
    Expect("{");
    std::map<std::string, TypePtr> types;
    ParseTypeMap(types, "}");
    return m_TypeCtx.GetStructType(types);
}

NJS::TypePtr NJS::Parser::ParseFunctionType()
{
    Expect("(");
    std::vector<TypePtr> args;
    const auto vararg = ParseTypeList(args, ")");
    TypePtr result;
    if (NextAt(":"))
        result = ParseType();
    else result = m_TypeCtx.GetVoidType();
    return m_TypeCtx.GetFunctionType(result, args, vararg);
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
