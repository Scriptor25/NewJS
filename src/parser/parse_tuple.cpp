#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TypeContext.hpp>

NJS::ExprPtr NJS::Parser::ParseTupleExpr()
{
    const auto where = Expect("[").Where;

    std::vector<ExprPtr> entries;
    std::vector<TypePtr> types;
    bool is_tuple = false;

    while (!At("]") && !AtEof())
    {
        const auto value = ParseExpr();
        entries.push_back(value);
        types.push_back(value->Type);

        if (!is_tuple && value->Type != types.front())
            is_tuple = true;

        if (!At("]"))
            Expect(",");
        else NextAt(",");
    }
    Expect("]");

    TypePtr type;
    if (is_tuple) type = m_Ctx.GetTupleType(types);
    else type = m_Ctx.GetArrayType(types.front(), types.size());

    return std::make_shared<TupleExpr>(where, type, entries);
}
