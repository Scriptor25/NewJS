#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Error.hpp>
#include <NJS/Param.hpp>
#include <NJS/Type.hpp>

NJS::ConstFunctionExpr::ConstFunctionExpr(
    SourceLocation where,
    TypePtr type,
    std::vector<ParamPtr> params,
    const bool vararg,
    TypePtr result_type,
    ScopeStmt body)
    : Expr(std::move(where), std::move(type)),
      Params(std::move(params)),
      VarArg(vararg),
      ResultType(std::move(result_type)),
      Body(std::move(body))
{
}

NJS::ValuePtr NJS::ConstFunctionExpr::GenLLVM(Builder& builder)
{
    Error(Where, "NJS::ConstFunctionExpr::GenLLVM");
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
        if (VarArg)
        {
            if (!Params.empty())
                os << ", ";
            os << "...";
        }
        ResultType->Print(os << "): ") << ' ';
    }
    return Body.Print(os);
}
