#include <llvm/IR/Verifier.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Param.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::FunctionStmt::FunctionStmt(std::string name, std::vector<ParamPtr> params, TypePtr type, ScopeStmtPtr body)
    : Name(std::move(name)), Params(std::move(params)), Type(std::move(type)), Body(std::move(body))
{
}

NJS::ValuePtr NJS::FunctionStmt::GenLLVM(Builder& builder)
{
    auto function = builder.LLVMModule().getFunction(builder.ValueName(Name));
    if (!function)
    {
        const auto type = llvm::dyn_cast<llvm::FunctionType>(Type->GenLLVM(builder));
        function = llvm::Function::Create(type, llvm::GlobalValue::ExternalLinkage, Name, builder.LLVMModule());

        builder.CreateVar(Name) = LValue::Create(builder, Type, function);
    }

    if (!Body) return {};
    if (!function->empty()) Error("cannot redefine function");

    const auto bkp = builder.LLVMBuilder().GetInsertBlock();
    builder.LLVMBuilder().SetInsertPoint(llvm::BasicBlock::Create(builder.LLVMContext(), "entry", function));

    builder.Push(Name);
    for (size_t i = 0; i < Params.size(); ++i)
    {
        const auto& param = Params[i];
        const auto arg = function->getArg(i);
        arg->setName(param->Name);
        const auto value = RValue::Create(builder, param->Type, arg);
        param->CreateVars(builder, false, value);
    }

    Body->GenLLVM(builder);
    builder.Pop();

    for (auto& bb : *function)
    {
        if (bb.getTerminator()) continue;
        if (function->getReturnType()->isVoidTy())
        {
            builder.LLVMBuilder().SetInsertPoint(&bb);
            builder.LLVMBuilder().CreateRetVoid();
            continue;
        }
        Error("not all code paths return");
    }

    if (verifyFunction(*function, &llvm::errs()))
        Error("failed to verify function");

    builder.LLVMBuilder().SetInsertPoint(bkp);
    return {};
}

std::ostream& NJS::FunctionStmt::Print(std::ostream& os)
{
    os << "function " << Name << "(";
    for (size_t i = 0; i < Params.size(); ++i)
    {
        if (i > 0) os << ", ";
        os << Params[i];
    }
    os << "): " << Type->Result();
    if (Body) Body->Print(os << ' ');
    return os;
}
