#include <NJS/AST.hpp>

NJS::ConstFunctionExpr::ConstFunctionExpr(TypePtr type, std::vector<ParamPtr> params, ScopeStmt body)
    : Expr(std::move(type)), Params(std::move(params)), Body(std::move(body))
{
}

NJS::ValuePtr NJS::ConstFunctionExpr::GenLLVM(Builder& builder)
{
}

std::ostream& NJS::ConstFunctionExpr::Print(std::ostream& os)
{
    os << '?';
    if (!Params.empty())
    {
        os << '(';
        for (size_t i = 0; i < Params.size(); ++i)
        {
            if (i > 0) os << ", ";
            os << Params[i];
        }
        os << ") ";
    }
    return Body.Print(os);
}
