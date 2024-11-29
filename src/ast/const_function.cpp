#include <NJS/AST.hpp>
#include <NJS/Error.hpp>
#include <NJS/Param.hpp>

NJS::ConstFunctionExpr::ConstFunctionExpr(std::vector<ParamPtr> params, ScopeStmt body)
    : Params(std::move(params)), Body(std::move(body))
{
}

NJS::ValuePtr NJS::ConstFunctionExpr::GenLLVM(Builder& builder)
{
    Error("NJS::ConstFunctionExpr::GenLLVM");
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
            Params[i]->Print(os);
        }
        os << ") ";
    }
    return Body.Print(os);
}
