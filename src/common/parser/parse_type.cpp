#include <newjs/parser.hpp>
#include <newjs/template_context.hpp>
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
    else if (const auto name = Expect(TokenType_Symbol).String; GetTemplateContext().HasType(name))
    {
        std::vector<TypePtr> arguments;

        Expect("<");
        while (!At(">"))
        {
            arguments.emplace_back(ParseType());
            if (!At(">"))
                Expect(",");
        }
        Expect(">");

        if (!m_IsTemplate)
            type = GetTemplateContext().InflateType(*this, name, arguments);
        else
            type = GetTypeContext().GetIncompleteType(name);
    }
    else
    {
        if (get_type_map.contains(name))
            type = get_type_map.at(name)(GetTypeContext());
        else
            try
            {
                type = GetTypeContext().GetType(name);
            }
            catch (const RTError &error)
            {
                Error(error, where, {});
            }
    }

    while (true)
    {
        if (NextAt("["))
        {
            if (At(TokenType_Int))
            {
                const auto count = Skip().Int;
                type = GetTypeContext().GetArrayType(type, count);
            }
            else
            {
                const auto is_const = NextAt("const");
                type = GetTypeContext().GetPointerType(type, is_const);
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
    return GetTypeContext().GetTupleType(element_types);
}

NJS::TypePtr NJS::Parser::ParseStructType()
{
    std::vector<StructElement> elements;

    Expect("{");
    while (!At("}"))
    {
        const auto is_const = NextAt("const");
        const auto is_reference = NextAt("&");
        const auto name = Expect(TokenType_Symbol).String;
        Expect(":");
        const auto type = ParseType();

        ExpressionPtr default_value;
        if ((!is_reference && is_const && (Expect("="), true)) || NextAt("="))
            default_value = ParseExpression();
        if (!is_reference && is_const)
            default_value = std::make_shared<CacheExpression>(default_value->Where, default_value);

        elements.emplace_back(name, ReferenceInfo(type, is_const, is_reference), default_value);

        if (!At("}"))
            Expect(",");
    }
    Expect("}");

    return GetTypeContext().GetStructType(elements);
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
        result.Type = GetTypeContext().GetVoidType();
    return GetTypeContext().GetFunctionType(result, parameters, is_var_arg);
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
