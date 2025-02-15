#include <NJS/AST.hpp>
#include <NJS/Error.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TemplateContext.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>

static std::string to_upper(const std::string_view &src)
{
    std::string string(src);
    for (auto &c: string)
        c = static_cast<char>(std::toupper(c));
    return string;
}

static void replace_all(std::string &src, const std::string_view &find, const std::string_view &replace)
{
    for (size_t pos; (pos = src.find(find)) != std::string::npos;)
        src.replace(pos, find.size(), replace);
}

NJS::ExpressionPtr NJS::Parser::ParsePrimaryExpression()
{
    const auto where = m_Token.Where;

    if (At(TokenType_Int))
    {
        const auto value = Skip().IntValue;
        TypePtr type;
        if (NextAt(":"))
            type = ParseType();
        return std::make_shared<IntegerExpression>(where, type, value);
    }

    if (At(TokenType_FP))
    {
        const auto value = Skip().FPValue;
        TypePtr type;
        if (NextAt(":"))
            type = ParseType();
        return std::make_shared<FloatingPointExpression>(where, type, value);
    }

    if (At(TokenType_String))
    {
        std::string value;
        do
            value += Skip().StringValue;
        while (At(TokenType_String));
        return std::make_shared<StringExpression>(where, value);
    }

    if (At(TokenType_Char))
        return std::make_shared<CharacterExpression>(where, Skip().StringValue[0]);

    if (NextAt("true"))
        return std::make_shared<BooleanExpression>(where, true);

    if (NextAt("false"))
        return std::make_shared<BooleanExpression>(where, false);

    if (NextAt("("))
    {
        const auto ptr = ParseExpression();
        Expect(")");
        return ptr;
    }

    if (At("{"))
        return ParseStructExpression();

    if (At("["))
        return ParseTupleExpression();

    if (At("?"))
        return ParseFunctionExpression();

    if (At("$"))
        return ParseFormatExpression();

    if (At("switch"))
        return ParseSwitchExpression();

    if (NextAt("sizeof"))
    {
        if (NextAt("("))
        {
            const auto expr = ParseExpression();
            Expect(")");
            return std::make_shared<SizeOfExpression>(where, expr);
        }

        Expect("<");
        const auto type = ParseType();
        Expect(">");
        return std::make_shared<IntegerExpression>(where, m_TypeContext.GetIntegerType(64, false), type->GetSize());
    }

    if (NextAt("typeof"))
    {
        Expect("(");
        const auto expr = ParseExpression();
        Expect(")");
        return std::make_shared<TypeOfExpression>(where, expr);
    }

    if (At(TokenType_Symbol))
    {
        const auto name = Skip().StringValue;

        if (m_MacroMap.contains(name))
        {
            const auto &[parameters_, source_] = m_MacroMap[name];

            auto source = source_;
            if (!parameters_.empty())
            {
                Expect("(");
                for (const auto &parameter: parameters_)
                {
                    std::string argument;
                    while (!At(",") && !At(")") && !AtEof())
                        argument += Skip().StringValue + ' ';
                    argument.pop_back();

                    replace_all(source, "##" + parameter, to_upper(argument));
                    replace_all(source, "#" + parameter, argument);

                    if (!At(")"))
                        Expect(",");
                }
                Expect(")");
            }

            std::stringstream stream(source);
            Parser parser(m_TypeContext, m_TemplateContext, stream, SourceLocation("<macro>"), m_MacroMap);
            return parser.ParseExpression();
        }

        if (m_TemplateContext.HasFunction(name))
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

            std::string inflated_name;
            if (!m_IsTemplate)
                inflated_name = m_TemplateContext.InflateFunctionTemplate(*this, name, args);
            else
                inflated_name = name;
            return std::make_shared<SymbolExpression>(where, inflated_name);
        }

        return std::make_shared<SymbolExpression>(where, name);
    }

    if (At(TokenType_Operator))
    {
        const auto op = Skip().StringValue;
        const auto operand = ParseOperandExpression();
        return std::make_shared<UnaryExpression>(where, op, true, operand);
    }

    Error(m_Token.Where, "unused token {}", m_Token);
}
