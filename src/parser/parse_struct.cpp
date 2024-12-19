#include <NJS/AST.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Parser.hpp>

NJS::ExprPtr NJS::Parser::ParseStructExpr()
{
    const auto where = Expect("{").Where;

    std::map<std::string, ExprPtr> entries;
    std::map<std::string, TypePtr> elements;
    while (!At("}") && !AtEof())
    {
        const auto [where_, _1, name_, _2, _3] = Expect(TokenType_Symbol);
        if (!NextAt(":"))
        {
            const auto type = GetVar(name_);
            entries[name_] = std::make_shared<SymbolExpr>(where_, type, name_);
            elements[name_] = type;
        }
        else
        {
            const auto value = ParseExpr();
            entries[name_] = value;
            elements[name_] = value->Type;
        }

        if (!At("}"))
            Expect(",");
        else NextAt(",");
    }
    Expect("}");

    const auto type = m_Ctx.GetStructType(elements);
    return std::make_shared<StructExpr>(where, type, entries);
}
