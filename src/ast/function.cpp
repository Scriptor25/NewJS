#include <llvm/IR/Verifier.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Context.hpp>
#include <NJS/Error.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Param.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::FunctionStmt::FunctionStmt(
    const bool extern_,
    std::string name,
    std::vector<ParamPtr> params,
    const bool vararg,
    TypePtr result_type,
    ScopeStmtPtr body)
    : Extern(extern_),
      Name(std::move(name)),
      Params(std::move(params)),
      VarArg(vararg),
      ResultType(std::move(result_type)),
      Body(std::move(body))
{
}

NJS::ValuePtr NJS::FunctionStmt::GenLLVM(Builder& builder)
{
    const auto name = Extern ? Name : builder.GetName(Name);
    auto function = builder.GetModule().getFunction(name);
    if (!function)
    {
        std::vector<TypePtr> param_types;
        for (const auto& param : Params)
            param_types.push_back(param->Type);
        const auto type = builder.GetCtx().GetFunctionType(param_types, ResultType, VarArg);
        const auto llvm_type = type->GenFnLLVM(builder);
        function = llvm::Function::Create(
            llvm_type,
            llvm::GlobalValue::ExternalLinkage,
            name,
            builder.GetModule());

        builder.DefVar(Name) = RValue::Create(builder, type, function);
    }

    if (!Body) return {};
    if (!function->empty()) Error("cannot redefine function");

    const auto return_block = builder.GetBuilder().GetInsertBlock();
    const auto entry_block = llvm::BasicBlock::Create(builder.GetContext(), "entry", function);
    builder.GetBuilder().SetInsertPoint(entry_block);

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

    for (auto& block : *function)
    {
        if (block.getTerminator()) continue;
        if (function->getReturnType()->isVoidTy())
        {
            builder.GetBuilder().SetInsertPoint(&block);
            builder.GetBuilder().CreateRetVoid();
            continue;
        }
        Error("not all code paths return a value: in function {} ({})", Name, name);
    }

    if (verifyFunction(*function, &llvm::errs()))
    {
        function->print(llvm::errs());
        Error("failed to verify function {} ({})", Name, name);
    }

    builder.GetBuilder().SetInsertPoint(return_block);
    return {};
}

std::ostream& NJS::FunctionStmt::Print(std::ostream& os)
{
    if (Extern) os << "extern";
    else os << "function";
    os << ' ' << Name << "(";
    for (size_t i = 0; i < Params.size(); ++i)
    {
        if (i > 0) os << ", ";
        Params[i]->Print(os);
    }
    if (VarArg)
    {
        if (!Params.empty()) os << ", ";
        os << "...";
    }
    ResultType->Print(os << "): ");
    if (Body) Body->Print(os << ' ');
    return os;
}
