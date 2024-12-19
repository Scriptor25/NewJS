#include <utility>
#include <llvm/IR/Verifier.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Param.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::FunctionStmt::FunctionStmt(
    SourceLocation where,
    const bool is_extern,
    std::string name,
    std::vector<ParamPtr> args,
    const bool vararg,
    TypePtr result_type,
    StmtPtr body)
    : Stmt(std::move(where)),
      Extern(is_extern),
      Name(std::move(name)),
      Args(std::move(args)),
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
        std::vector<TypePtr> args;
        for (const auto& arg : Args)
            args.push_back(arg->Type);
        const auto type = builder.GetCtx().GetFunctionType(ResultType, args, VarArg);
        function = llvm::Function::Create(
            type->GenFnLLVM(builder),
            llvm::GlobalValue::ExternalLinkage,
            name,
            builder.GetModule());

        builder.DefVar(Name) = RValue::Create(builder, type, function);
    }

    if (!Body) return {};
    if (!function->empty()) Error(Where, "redefining function {} ({})", Name, name);

    const auto end_block = builder.GetBuilder().GetInsertBlock();
    const auto entry_block = llvm::BasicBlock::Create(builder.GetContext(), "entry", function);
    builder.GetBuilder().SetInsertPoint(entry_block);

    builder.Push(Name);
    for (unsigned i = 0; i < Args.size(); ++i)
    {
        const auto& arg = Args[i];
        const auto ir_arg = function->getArg(i);
        ir_arg->setName(arg->Name);

        ValuePtr value;
        if (arg->Type->IsRef())
            value = LValue::Create(builder, arg->Type->GetElement(), ir_arg);
        else value = RValue::Create(builder, arg->Type, ir_arg);
        arg->CreateVars(builder, false, value);
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
        Error(Where, "not all code paths return a value: in function {} ({})", Name, name);
    }

    if (verifyFunction(*function, &llvm::errs()))
    {
        function->print(llvm::errs());
        Error(Where, "failed to verify function {} ({})", Name, name);
    }

    builder.GetBuilder().SetInsertPoint(end_block);
    return {};
}

std::ostream& NJS::FunctionStmt::Print(std::ostream& os)
{
    if (Extern) os << "extern";
    else os << "function";
    os << ' ' << Name << "(";
    for (unsigned i = 0; i < Args.size(); ++i)
    {
        if (i > 0) os << ", ";
        Args[i]->Print(os);
    }
    if (VarArg)
    {
        if (!Args.empty()) os << ", ";
        os << "...";
    }
    ResultType->Print(os << "): ");
    if (Body) Body->Print(os << ' ');
    return os;
}

NJS::FunctionExpr::FunctionExpr(
    SourceLocation where,
    TypePtr type,
    std::vector<ParamPtr> args,
    const bool vararg,
    TypePtr result_type,
    StmtPtr body)
    : Expr(std::move(where), std::move(type)),
      Args(std::move(args)),
      VarArg(vararg),
      ResultType(std::move(result_type)),
      Body(std::move(body))
{
}

NJS::ValuePtr NJS::FunctionExpr::GenLLVM(Builder& builder)
{
    Error(Where, "NJS::ConstFunctionExpr::GenLLVM");
}

std::ostream& NJS::FunctionExpr::Print(std::ostream& os)
{
    os << '?';
    if (!Args.empty())
    {
        os << '(';
        for (unsigned i = 0; i < Args.size(); ++i)
        {
            if (i > 0) os << ", ";
            Args[i]->Print(os);
        }
        if (VarArg)
        {
            if (!Args.empty())
                os << ", ";
            os << "...";
        }
        ResultType->Print(os << "): ") << ' ';
    }
    return Body->Print(os);
}
