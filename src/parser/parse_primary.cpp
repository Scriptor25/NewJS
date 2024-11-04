#include <iostream>
#include <sstream>
#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>

#include "NJS/Context.hpp"
#include "NJS/NJS.hpp"
#include "NJS/Param.hpp"

NJS::ExprPtr NJS::Parser::ParsePrimary()
{
    if (At(TokenType_Symbol))
    {
        const auto name = Skip().StringValue;
        const auto type = m_Ctx.GetVar(name);
        return std::make_shared<SymbolExpr>(type, name);
    }

    if (At(TokenType_Number))
        return std::make_shared<ConstNumberExpr>(m_Ctx.GetNumberType(), Skip().NumberValue);

    if (At(TokenType_String))
        return std::make_shared<ConstStringExpr>(m_Ctx.GetStringType(), Skip().StringValue);

    if (At(TokenType_Operator))
    {
        const auto op = Skip().StringValue;
        auto operand = ParseOperand();

        TypePtr type; // TODO: get operator result
        return std::make_shared<UnaryExpr>(type, op, false, operand);
    }

    if (NextAt("("))
    {
        const auto ptr = ParseExpression();
        Expect(")");
        return ptr;
    }

    if (NextAt("{"))
    {
        std::map<std::string, ExprPtr> entries;
        std::map<std::string, TypePtr> types;
        while (!NextAt("}"))
        {
            const auto name = Expect(TokenType_Symbol).StringValue;
            if (!NextAt(":"))
            {
                const auto type = m_Ctx.GetVar(name);
                entries[name] = std::make_shared<SymbolExpr>(type, name);
                types[name] = type;
            }
            else
            {
                const auto value = ParseExpression();
                entries[name] = value;
                types[name] = value->Type;
            }

            if (!At("}"))
                Expect(",");
            else NextAt(",");
        }
        return std::make_shared<ConstObjectExpr>(m_Ctx.GetObjectType(types), entries);
    }

    if (NextAt("["))
    {
        std::vector<ExprPtr> entries;
        std::vector<TypePtr> types;
        while (!NextAt("]"))
        {
            const auto value = ParseExpression();
            entries.push_back(value);
            types.push_back(value->Type);

            if (!At("]"))
                Expect(",");
            else NextAt(",");
        }

        auto first = types.front();
        bool same = true;
        for (const auto& type : types)
            if (first != type)
            {
                same = false;
                break;
            }

        TypePtr type;
        if (same) type = m_Ctx.GetArrayType(first);
        else type = m_Ctx.GetTupleType(types);

        return std::make_shared<ConstTupleExpr>(type, entries);
    }

    if (At("$"))
    {
        auto where = Skip().Where;
        auto source = Expect(TokenType_String).StringValue;
        std::map<size_t, std::string> statics;
        std::map<size_t, ExprPtr> dynamics;
        size_t index = 0;
        for (size_t beg; (beg = source.find('{')) != std::string::npos;)
        {
            if (source[beg + 1] == '{')
            {
                const auto str = source.substr(0, beg + 1);
                statics[index++] = str;
                source.erase(0, beg + 2);
                continue;
            }
            if (const auto str = source.substr(0, beg); !str.empty()) statics[index++] = str;
            source.erase(0, beg + 1);

            std::stringstream stream(source);
            Parser parser(m_Ctx, stream, "<dynamic>");
            dynamics[index++] = Catch<ExprPtr>(
                [&] { return parser.ParseExpression(); },
                [&] { Error(where); });

            source.erase(0, static_cast<size_t>(stream.tellg()) - 1);
        }
        if (!source.empty()) statics[index++] = source;
        return std::make_shared<FormatExpr>(m_Ctx.GetStringType(), index, statics, dynamics);
    }

    if (NextAt("?"))
    {
        std::vector<ParamPtr> params;
        bool vararg = false;
        if (NextAt("("))
            vararg = ParseParamList(params, ")");

        TypePtr result_type;
        if (NextAt(": "))
            result_type = ParseType();
        else result_type = m_Ctx.GetVoidType();

        m_Ctx.StackPush();
        std::vector<TypePtr> param_types;
        for (const auto& param : params)
        {
            param->CreateVars(m_Ctx, {});
            param_types.push_back(param->Type);
        }
        const auto body = ParseScope();
        m_Ctx.StackPop();

        const auto type = m_Ctx.GetFunctionType(param_types, result_type, vararg);
        return std::make_shared<ConstFunctionExpr>(type, params, *body);
    }

    Error(m_Token.Where, "unused token {}", m_Token);
}
