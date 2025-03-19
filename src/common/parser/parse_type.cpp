#include <newjs/parameter.hpp>
#include <newjs/parser.hpp>
#include <newjs/type_context.hpp>

NJS::TypePtr NJS::Parser::ParseType()
{
    static const std::map<std::string, std::function<TypePtr(TypeContext &)>> get_type_map
    {
        {
            "void",
            [](TypeContext &context)
            {
                return context.GetVoidType();
            }
        },
        {
            "boolean",
            [](TypeContext &context)
            {
                return context.GetBooleanType();
            },
        },
        {
            "char",
            [](TypeContext &context)
            {
                return context.GetCharType();
            },
        },
        {
            "string",
            [](TypeContext &context)
            {
                return context.GetStringType();
            },
        },
        {
            "i1",
            [](TypeContext &context)
            {
                return context.GetIntegerType(1, true);
            },
        },
        {
            "i8",
            [](TypeContext &context)
            {
                return context.GetIntegerType(8, true);
            },
        },
        {
            "i16",
            [](TypeContext &context)
            {
                return context.GetIntegerType(16, true);
            },
        },
        {
            "i32",
            [](TypeContext &context)
            {
                return context.GetIntegerType(32, true);
            },
        },
        {
            "i64",
            [](TypeContext &context)
            {
                return context.GetIntegerType(64, true);
            },
        },
        {
            "u1",
            [](TypeContext &context)
            {
                return context.GetIntegerType(1, false);
            },
        },
        {
            "u8",
            [](TypeContext &context)
            {
                return context.GetIntegerType(8, false);
            },
        },
        {
            "u16",
            [](TypeContext &context)
            {
                return context.GetIntegerType(16, false);
            },
        },
        {
            "u32",
            [](TypeContext &context)
            {
                return context.GetIntegerType(32, false);
            },
        },
        {
            "u64",
            [](TypeContext &context)
            {
                return context.GetIntegerType(64, false);
            },
        },
        {
            "f16",
            [](TypeContext &context)
            {
                return context.GetFloatingPointType(16);
            },
        },
        {
            "f32",
            [](TypeContext &context)
            {
                return context.GetFloatingPointType(32);
            },
        },
        {
            "f64",
            [](TypeContext &context)
            {
                return context.GetFloatingPointType(64);
            },
        },
    };

    const auto where = CurrentLocation();

    TypePtr type;
    if (At("["))
        type = ParseTupleType();
    else if (At("{"))
        type = ParseStructType();
    else if (At("("))
        type = ParseFunctionType();
    else if (At("lambda"))
        type = ParseLambdaType();
    else if (const auto name = Expect(TokenType_Symbol).String; get_type_map.contains(name))
        type = get_type_map.at(name)(m_TypeContext);
    else if (m_MacroMap.contains(name))
        type = m_MacroMap.at(name).InflateType(*this);
    else
        try
        {
            type = m_TypeContext.GetNamedType(name);
        }
        catch (const RTError &error)
        {
            Error(error, where, {});
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
    std::vector<StructElement> struct_elements;

    Expect("{");
    while (!At("}"))
    {
        const auto is_const = NextAt("const");
        const auto is_reference = NextAt("&");
        const auto name = Expect(TokenType_Symbol).String;
        Expect(":");
        const auto type = ParseType();

        ExpressionPtr default_value;
        if (NextAt("="))
        {
            default_value = ParseExpression();
            if (!is_reference && is_const)
                default_value = std::make_shared<CacheExpression>(default_value->Where, default_value);
        }

        struct_elements.emplace_back(name, ReferenceInfo(type, is_const, is_reference), default_value);

        if (!At("}"))
            Expect(",");
    }
    Expect("}");

    std::string struct_name;
    if (NextAt("."))
        struct_name = Expect(TokenType_Symbol).String;

    return m_TypeContext.GetStructType(struct_elements, struct_name);
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

NJS::TypePtr NJS::Parser::ParseLambdaType()
{
    Expect("lambda");
    Expect("[");
    std::vector<StructElement> elements;
    while (!At("]"))
    {
        const auto is_const = NextAt("const");
        const auto is_reference = NextAt("&");
        const auto name = Expect(TokenType_Symbol).String;
        Expect(":");
        const auto type = ParseType();

        elements.emplace_back(name, ReferenceInfo(type, is_const, is_reference), nullptr);

        if (!At("]"))
            Expect(",");
    }
    Expect("]");
    Expect("<");
    const auto function_type = Type::As<FunctionType>(ParseFunctionType());
    Expect(">");

    return m_TypeContext.GetLambdaType(elements, function_type);
}

NJS::ReferenceInfo NJS::Parser::ParseReferenceInfo()
{
    ReferenceInfo info;
    info.IsConst = NextAt("const");
    info.IsReference = NextAt("&");
    info.Type = ParseType();
    return info;
}

bool NJS::Parser::ParseTypeList(std::vector<TypePtr> &types, const std::string &delimiter)
{
    while (!At(delimiter))
    {
        if (NextAt("..."))
        {
            Expect(delimiter);
            return true;
        }
        types.emplace_back(ParseType());
        if (!At(delimiter))
            Expect(",");
    }
    Expect(delimiter);
    return false;
}

void NJS::Parser::ParseTypeMap(std::vector<std::pair<std::string, TypePtr>> &types, const std::string &delimiter)
{
    while (!At(delimiter))
    {
        const auto name = Expect(TokenType_Symbol).String;
        Expect(":");
        types.emplace_back(name, ParseType());

        if (!At(delimiter))
            Expect(",");
    }
    Expect(delimiter);
}

bool NJS::Parser::ParseReferenceInfoList(std::vector<ReferenceInfo> &infos, const std::string &delimiter)
{
    while (!At(delimiter))
    {
        if (NextAt("..."))
        {
            Expect(delimiter);
            return true;
        }
        infos.emplace_back(ParseReferenceInfo());
        if (!At(delimiter))
            Expect(",");
    }
    Expect(delimiter);
    return false;
}
