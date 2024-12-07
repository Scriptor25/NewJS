#include <NJS/AST.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Parser.hpp>

NJS::ExprPtr NJS::Parser::ParseConstObject()
{
    const auto where = Expect("{").Where;

    std::map<std::string, ExprPtr> entries;
    std::map<std::string, TypePtr> types;
    while (!At("}") && !AtEof())
    {
        const auto [where_, type_, name_, number_] = Expect(TokenType_Symbol);
        if (!NextAt(":"))
        {
            const auto type = GetVar(name_);
            entries[name_] = std::make_shared<SymbolExpr>(where_, type, name_);
            types[name_] = type;
        }
        else
        {
            const auto value = ParseExpression();
            entries[name_] = value;
            types[name_] = value->Type;
        }

        if (!At("}"))
            Expect(",");
        else NextAt(",");
    }
    Expect("}");

    const auto type = m_Ctx.GetObjectType(types);
    return std::make_shared<ConstObjectExpr>(where, type, entries);
}
