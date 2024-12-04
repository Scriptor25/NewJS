#include <llvm/IR/Value.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::CallExpr::CallExpr(ExprPtr callee, std::vector<ExprPtr> args)
    : Callee(std::move(callee)), Args(std::move(args))
{
}

NJS::ValuePtr NJS::CallExpr::GenLLVM(Builder& builder)
{
    const auto callee = Callee->GenLLVM(builder);

    std::vector<llvm::Value*> args(Args.size());
    for (size_t i = 0; i < Args.size(); ++i)
        args[i] = Args[i]->GenLLVM(builder)->Load();

    const auto callee_type = std::dynamic_pointer_cast<FunctionType>(callee->GetType());
    const auto value = builder.LLVMBuilder().CreateCall(
        callee_type->GenFnLLVM(builder),
        callee->Load(),
        args);
    return RValue::Create(builder, callee_type->Result(), value);
}

std::ostream& NJS::CallExpr::Print(std::ostream& os)
{
    Callee->Print(os) << '(';
    for (size_t i = 0; i < Args.size(); ++i)
    {
        if (i > 0) os << ", ";
        Args[i]->Print(os);
    }
    return os << ')';
}
