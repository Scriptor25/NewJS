#include <NJS/AST.hpp>
#include <NJS/Error.hpp>
#include <NJS/Param.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TemplateContext.hpp>
#include <NJS/TypeContext.hpp>

NJS::StmtPtr NJS::Parser::ParseFunctionStmt()
{
    const auto where = m_Token.Where;

    const auto fn = NextAt("extern")
                        ? FnType_Extern
                        : NextAt("operator")
                              ? FnType_Operator
                              : NextAt("template")
                                    ? FnType_Template
                                    : (Expect("function"), FnType_Function);

    std::vector<std::string> templ_args;
    const auto parent_template = m_IsTemplate;
    if (fn == FnType_Template)
    {
        m_IsTemplate = true;

        Expect("<");
        while (!At(">") && !AtEof())
        {
            templ_args.push_back(Expect(TokenType_Symbol).StringValue);

            if (!At(">"))
                Expect(",");
        }
        Expect(">");

        if (!parent_template)
            ResetBuffer();
    }

    const auto name = (fn == FnType_Operator ? Expect(TokenType_Operator) : Expect(TokenType_Symbol)).StringValue;

    std::vector<ParamPtr> args;
    Expect("(");
    const auto vararg = ParseParamList(args, ")");

    TypePtr result_type;
    if (NextAt(":"))
        result_type = ParseType();
    else
        result_type = m_TypeContext.GetVoidType();

    StmtPtr body;
    if (fn != FnType_Extern && At("{"))
        body = ParseScopeStmt();

    if (fn == FnType_Template)
    {
        if (parent_template)
            return {};

        m_IsTemplate = false;
        m_TemplateContext.InsertFunction(name, templ_args, m_TemplateWhere, m_TemplateBuffer.str());
    }

    return std::make_shared<FunctionStmt>(where, false, fn, name, args, vararg, result_type, body);
}

NJS::ExprPtr NJS::Parser::ParseFunctionExpr()
{
    const auto where = Expect("?").Where;

    std::vector<ParamPtr> args;
    bool vararg = false;
    if (NextAt("("))
        vararg = ParseParamList(args, ")");

    TypePtr result_type;
    if (NextAt(":"))
        result_type = ParseType();
    else
        result_type = m_TypeContext.GetVoidType();

    std::vector<TypePtr> arg_types;
    for (const auto &arg: args)
        arg_types.push_back(arg->Type);

    const auto body = ParseScopeStmt();

    return std::make_shared<FunctionExpr>(where, args, vararg, result_type, body);
}
