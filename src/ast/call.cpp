#include <utility>
#include <llvm/IR/Value.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::CallExpr::CallExpr(SourceLocation where, ExprPtr callee, std::vector<ExprPtr> args)
    : Expr(std::move(where)), Callee(std::move(callee)), Args(std::move(args))
{
}

NJS::ValuePtr NJS::CallExpr::GenLLVM(Builder& builder, const TypePtr& expected) const
{
    const auto callee = Callee->GenLLVM(builder, {});
    const auto callee_type = std::dynamic_pointer_cast<FunctionType>(callee->GetType());
    if (!callee_type)
        Error(Where, "invalid callee: callee is not a function");

    std::vector<llvm::Value*> arg_values(Args.size());
    for (unsigned i = 0; i < Args.size(); ++i)
    {
        auto param_type = callee_type->Param(i);
        auto& arg = Args[i];
        auto arg_value = arg->GenLLVM(builder, param_type);

        const auto param_is_ref = param_type->IsRef();
        if (param_is_ref) param_type = param_type->GetElement();

        arg_value = builder.CreateCast(arg->Where, arg_value, param_type);

        arg_values[i] = param_is_ref
                            ? arg_value->GetPtr(arg->Where)
                            : arg_value->Load(arg->Where);
    }

    const auto result_value = builder.GetBuilder().CreateCall(
        callee_type->GenFnLLVM(Where, builder),
        callee->Load(Where),
        arg_values);

    if (callee_type->GetResult()->IsRef())
        return LValue::Create(builder, callee_type->GetResult()->GetElement(), result_value);

    return RValue::Create(builder, callee_type->GetResult(), result_value);
}

std::ostream& NJS::CallExpr::Print(std::ostream& os)
{
    Callee->Print(os) << '(';
    for (unsigned i = 0; i < Args.size(); ++i)
    {
        if (i > 0) os << ", ";
        Args[i]->Print(os);
    }
    return os << ')';
}
