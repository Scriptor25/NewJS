#include <iostream>
#include <sstream>
#include <NJS/AST.hpp>
#include <NJS/Context.hpp>
#include <NJS/Error.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Parser.hpp>

NJS::ExprPtr NJS::Parser::ParsePrimary()
{
    if (At(TokenType_Symbol))
    {
        const auto name = Skip().StringValue;
        return std::make_shared<SymbolExpr>(name);
    }

    if (At(TokenType_Number))
        return std::make_shared<ConstNumberExpr>(Skip().NumberValue);

    if (At(TokenType_String))
        return std::make_shared<ConstStringExpr>(Skip().StringValue);

    if (At(TokenType_Operator))
    {
        const auto op = Skip().StringValue;
        auto operand = ParseOperand();
        return std::make_shared<UnaryExpr>(op, false, operand);
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
        while (!NextAt("}"))
        {
            const auto name = Expect(TokenType_Symbol).StringValue;
            if (!NextAt(":"))
                entries[name] = std::make_shared<SymbolExpr>(name);
            else
            {
                const auto value = ParseExpression();
                entries[name] = value;
            }

            if (!At("}"))
                Expect(",");
            else NextAt(",");
        }
        return std::make_shared<ConstObjectExpr>(entries);
    }

    if (NextAt("["))
    {
        std::vector<ExprPtr> entries;
        while (!NextAt("]"))
        {
            const auto value = ParseExpression();
            entries.push_back(value);

            if (!At("]"))
                Expect(",");
            else NextAt(",");
        }
        return std::make_shared<ConstTupleExpr>(entries);
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
            dynamics[index++] = parser.ParseExpression();

            source.erase(0, static_cast<size_t>(stream.tellg()) - 1);
        }
        if (!source.empty()) statics[index++] = source;
        return std::make_shared<FormatExpr>(index, statics, dynamics);
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

        const auto body = ParseScope();
        return std::make_shared<ConstFunctionExpr>(params, *body);
    }

    Error(m_Token.Where, "unused token {}", m_Token);
}
