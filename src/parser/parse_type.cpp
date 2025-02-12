#include <NJS/Error.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TemplateContext.hpp>
#include <NJS/TypeContext.hpp>

NJS::TypePtr NJS::Parser::ParseType()
{
    const auto where = m_Token.Where;

    TypePtr type;
    if (At("["))
        type = ParseTupleType();
    else if (At("{"))
        type = ParseStructType();
    else if (At("("))
        type = ParseFunctionType();
    else if (const auto sym = Expect(TokenType_Symbol).StringValue; m_TemplateContext.HasType(sym))
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
            type = m_TemplateContext.InflateType(*this, sym, args);
        else
            type = m_TypeContext.GetNoType(sym);
    }
    else if (!((type = m_TypeContext.GetType(sym))))
    {
        if (sym == "void")
            type = m_TypeContext.GetVoidType();
        else if (sym == "bool")
            type = m_TypeContext.GetBoolType();
        else if (sym == "char")
            type = m_TypeContext.GetCharType();
        else if (sym == "str")
            type = m_TypeContext.GetStringType();
        else if (sym == "i1")
            type = m_TypeContext.GetIntegerType(1, true);
        else if (sym == "i8")
            type = m_TypeContext.GetIntegerType(8, true);
        else if (sym == "i16")
            type = m_TypeContext.GetIntegerType(16, true);
        else if (sym == "i32")
            type = m_TypeContext.GetIntegerType(32, true);
        else if (sym == "i64")
            type = m_TypeContext.GetIntegerType(64, true);
        else if (sym == "u1")
            type = m_TypeContext.GetIntegerType(1, false);
        else if (sym == "u8")
            type = m_TypeContext.GetIntegerType(8, false);
        else if (sym == "u16")
            type = m_TypeContext.GetIntegerType(16, false);
        else if (sym == "u32")
            type = m_TypeContext.GetIntegerType(32, false);
        else if (sym == "u64")
            type = m_TypeContext.GetIntegerType(64, false);
        else if (sym == "f16")
            type = m_TypeContext.GetFloatingPointType(16);
        else if (sym == "f32")
            type = m_TypeContext.GetFloatingPointType(32);
        else if (sym == "f64")
            type = m_TypeContext.GetFloatingPointType(64);
        else
            type = m_TypeContext.GetNoType(sym);
    }

    while (true)
    {
        if (NextAt("["))
        {
            if (At(TokenType_Int))
            {
                const auto count = Skip().IntValue;
                type = m_TypeContext.GetArrayType(type, count);
            }
            else
                type = m_TypeContext.GetPointerType(type);
            Expect("]");
            continue;
        }

        if (NextAt("&"))
        {
            type = m_TypeContext.GetReferenceType(type);
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
    return m_TypeContext.GetTupleType(types);
}

NJS::TypePtr NJS::Parser::ParseStructType()
{
    Expect("{");
    std::map<std::string, TypePtr> types;
    ParseTypeMap(types, "}");
    return m_TypeContext.GetStructType(types);
}

NJS::TypePtr NJS::Parser::ParseFunctionType()
{
    Expect("(");
    std::vector<TypePtr> args;
    const auto vararg = ParseTypeList(args, ")");
    TypePtr result;
    if (NextAt("=>"))
        result = ParseType();
    else
        result = m_TypeContext.GetVoidType();
    return m_TypeContext.GetFunctionType(result, args, vararg);
}

bool NJS::Parser::ParseTypeList(std::vector<TypePtr> &types, const std::string_view &delim)
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
        else
            NextAt(",");
    }
    Expect(delim);
    return false;
}

void NJS::Parser::ParseTypeMap(std::map<std::string, TypePtr> &types, const std::string_view &delim)
{
    while (!At(delim) && !AtEof())
    {
        const auto name = Expect(TokenType_Symbol).StringValue;
        Expect(":");
        types[name] = ParseType();

        if (!At(delim))
            Expect(",");
        else
            NextAt(",");
    }
    Expect(delim);
}
