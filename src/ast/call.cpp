#include <utility>
#include <llvm/IR/Value.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::CallExpr::CallExpr(SourceLocation where, ExprPtr callee, std::vector<ExprPtr> args)
    : Expr(std::move(where)), Callee(std::move(callee)), Args(std::move(args))
{
}

NJS::ValuePtr NJS::CallExpr::GenLLVM(Builder& builder)
{
    const auto callee = Callee->GenLLVM(builder);
    const auto callee_type = std::dynamic_pointer_cast<FunctionType>(callee->GetType());
    if (!callee_type)
        Error(Where, "invalid callee: callee is not a function");

    std::vector<llvm::Value*> args(Args.size());
    for (unsigned i = 0; i < Args.size(); ++i)
    {
        auto arg = Args[i]->GenLLVM(builder);
        auto arg_type = callee_type->Arg(i);

        const auto ref = arg_type->IsRef();
        if (ref) arg_type = arg_type->GetElement();

        if (arg_type->IsPtr() && arg->GetType()->IsArray() &&
            arg_type->GetElement() == arg->GetType()->GetElement())
        {
            const auto ptr = arg->GetPtr();
            const auto gep = builder.GetBuilder().CreateConstGEP2_64(arg->GetType()->GetLLVM(builder), ptr, 0, 0);
            arg = RValue::Create(builder, builder.GetCtx().GetPointerType(arg->GetType()->GetElement()), gep);
        }

        if (arg->GetType() != arg_type)
            Error(Where, "invalid arg: type mismatch, {} != {}", arg->GetType(), arg_type);
        args[i] = ref ? arg->GetPtr() : arg->Load();
    }

    const auto value = builder.GetBuilder().CreateCall(
        callee_type->GenFnLLVM(builder),
        callee->Load(),
        args);
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
