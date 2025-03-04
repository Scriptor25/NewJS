#include <newjs/parser.hpp>
#include <newjs/template_context.hpp>
#include <newjs/type_context.hpp>

NJS::TypePtr NJS::Parser::ParseType()
{
    const auto where = CurrentLocation();

    TypePtr type;
    if (At("["))
        type = ParseTupleType();
    else if (At("{"))
        type = ParseStructType();
    else if (At("("))
        type = ParseFunctionType();
    else if (const auto sym = Expect(TokenType_Symbol).String; m_TemplateContext.HasType(sym))
    {
        std::vector<TypePtr> arguments;

        Expect("<");
        while (!At(">") && !AtEof())
        {
            arguments.emplace_back(ParseType());
            if (!At(">"))
                Expect(",");
        }
        Expect(">");

        if (!m_IsTemplate)
            type = m_TemplateContext.InflateType(*this, sym, arguments);
        else
            type = m_TypeContext.GetIncompleteType(sym);
    }
    else
    {
        if (sym == "void")
            type = m_TypeContext.GetVoidType();
        else if (sym == "boolean")
            type = m_TypeContext.GetBooleanType();
        else if (sym == "char")
            type = m_TypeContext.GetCharType();
        else if (sym == "string")
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
            type = m_TypeContext.GetType(where, sym);
    }

    while (true)
    {
        if (NextAt("["))
        {
            if (At(TokenType_Int))
            {
                const auto count = Skip().Int;
                type = m_TypeContext.GetArrayType(type, count);
            }
            else
            {
                const auto is_const = NextAt("const");
                type = m_TypeContext.GetPointerType(type, is_const);
            }
            Expect("]");
            continue;
        }
        break;
    }

    return type;
}

NJS::TypePtr NJS::Parser::ParseTupleType()
{
    Expect("[");
    std::vector<TypePtr> element_types;
    ParseTypeList(element_types, "]");
    return m_TypeContext.GetTupleType(element_types);
}

NJS::TypePtr NJS::Parser::ParseStructType()
{
    Expect("{");
    std::vector<std::pair<std::string, TypePtr>> element_types;
    ParseTypeMap(element_types, "}");
    return m_TypeContext.GetStructType(element_types);
}

NJS::TypePtr NJS::Parser::ParseFunctionType()
{
    Expect("(");
    std::vector<ReferenceInfo> parameters;
    const auto is_var_arg = ParseReferenceInfoList(parameters, ")");
    ReferenceInfo result;
    if (NextAt("=>"))
        result = ParseReferenceInfo();
    else
        result.Type = m_TypeContext.GetVoidType();
    return m_TypeContext.GetFunctionType(result, parameters, is_var_arg);
}

NJS::ReferenceInfo NJS::Parser::ParseReferenceInfo()
{
    ReferenceInfo info;
    info.IsConst = NextAt("const");
    info.IsReference = NextAt("&");
    info.Type = ParseType();
    return info;
}

bool NJS::Parser::ParseTypeList(std::vector<TypePtr> &types, const std::string &delim)
{
    while (!At(delim) && !AtEof())
    {
        if (NextAt("..."))
        {
            Expect(delim);
            return true;
        }
        types.emplace_back(ParseType());
        if (!At(delim))
            Expect(",");
    }
    Expect(delim);
    return false;
}

void NJS::Parser::ParseTypeMap(std::vector<std::pair<std::string, TypePtr>> &types, const std::string &delim)
{
    while (!At(delim) && !AtEof())
    {
        const auto name = Expect(TokenType_Symbol).String;
        Expect(":");
        types.emplace_back(name, ParseType());

        if (!At(delim))
            Expect(",");
    }
    Expect(delim);
}

bool NJS::Parser::ParseReferenceInfoList(std::vector<ReferenceInfo> &infos, const std::string &delim)
{
    while (!At(delim) && !AtEof())
    {
        if (NextAt("..."))
        {
            Expect(delim);
            return true;
        }
        infos.emplace_back(ParseReferenceInfo());
        if (!At(delim))
            Expect(",");
    }
    Expect(delim);
    return false;
}
