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

NJS::ValuePtr NJS::CallExpr::GenLLVM(Builder& builder, const TypePtr& expected)
{
    const auto callee = Callee->GenLLVM(builder, {});
    const auto callee_type = std::dynamic_pointer_cast<FunctionType>(callee->GetType());
    if (!callee_type)
        Error(Where, "invalid callee: callee is not a function");

    std::vector<llvm::Value*> args(Args.size());
    for (unsigned i = 0; i < Args.size(); ++i)
    {
        auto arg_type = callee_type->Arg(i);
        auto arg = Args[i]->GenLLVM(builder, arg_type);

        const auto ref = arg_type->IsRef();
        if (ref) arg_type = arg_type->GetElement();

        arg = builder.CreateCast(Where, arg, arg_type);

        args[i] = ref ? arg->GetPtr(Where) : arg->Load(Where);
    }

    const auto value = builder.GetBuilder().CreateCall(
        callee_type->GenFnLLVM(Where, builder),
        callee->Load(Where),
        args);

    if (callee_type->GetResult()->IsRef())
        return LValue::Create(builder, callee_type->GetResult()->GetElement(), value);

    return RValue::Create(builder, callee_type->GetResult(), value);
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
