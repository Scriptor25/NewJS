#include <utility>
#include <llvm/IR/Verifier.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Param.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::FunctionStmt::FunctionStmt(
    SourceLocation where,
    const bool absolute,
    const FunctionID fn_id,
    std::string name,
    std::vector<ParamPtr> args,
    const bool var_arg,
    TypePtr result_type,
    StmtPtr body)
    : Stmt(std::move(where)),
      Absolute(absolute),
      FnID(fn_id),
      Name(std::move(name)),
      Args(std::move(args)),
      VarArg(var_arg),
      ResultType(std::move(result_type)),
      Body(std::move(body))
{
}

void NJS::FunctionStmt::GenVoidLLVM(Builder &builder) const
{
    std::string function_name;
    switch (FnID)
    {
        case FnType_Function:
            function_name = builder.GetName(Absolute, Name);
            break;
        case FnType_Extern:
            function_name = Name;
            break;
        case FnType_Operator:
            switch (Args.size())
            {
                case 1:
                    function_name = builder.GetName(Absolute, Args[0]->Type->GetString() + Name);
                    break;
                case 2:
                    function_name = builder.GetName(
                        Absolute,
                        Args[0]->Type->GetString() + Name + Args[1]->Type->GetString());
                    break;
                default:
                    break;
            }
            break;
        case FnType_Template:
            return;
    }

    auto function = builder.GetModule().getFunction(function_name);
    if (!function)
    {
        std::vector<TypePtr> args;
        for (const auto &arg: Args)
            args.push_back(arg->Type);
        const auto type = builder.GetTypeContext().GetFunctionType(ResultType, args, VarArg);
        function = llvm::Function::Create(
            type->GenFnLLVM(Where, builder),
            llvm::GlobalValue::ExternalLinkage,
            function_name,
            builder.GetModule());

        switch (FnID)
        {
            case FnType_Function:
            case FnType_Extern:
                builder.DefineVariable(Where, Name) = RValue::Create(builder, type, function);
                break;
            case FnType_Operator:
                switch (Args.size())
                {
                    case 1:
                        builder.DefineOperator(Name, Args[0]->Type, ResultType, function);
                        break;
                    case 2:
                        builder.DefineOperator(Name, Args[0]->Type, Args[1]->Type, ResultType, function);
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    if (!Body)
        return;
    if (!function->empty())
        Error(Where, "redefining function {} ({})", Name, function_name);

    const auto end_block = builder.GetBuilder().GetInsertBlock();
    const auto entry_block = llvm::BasicBlock::Create(builder.GetContext(), "entry", function);
    builder.GetBuilder().SetInsertPoint(entry_block);

    builder.StackPush(Name, ResultType);
    for (unsigned i = 0; i < Args.size(); ++i)
    {
        const auto &arg = Args[i];
        const auto llvm_arg = function->getArg(i);
        llvm_arg->setName(arg->Name);

        ValuePtr arg_value;
        if (arg->Type->IsRef())
            arg_value = LValue::Create(builder, arg->Type->GetElement(), llvm_arg);
        else
            arg_value = RValue::Create(builder, arg->Type, llvm_arg);
        arg->CreateVars(builder, Where, false, arg_value);
    }

    Body->GenVoidLLVM(builder);
    builder.StackPop();

    for (auto &block: *function)
    {
        if (block.getTerminator())
            continue;
        if (function->getReturnType()->isVoidTy())
        {
            builder.GetBuilder().SetInsertPoint(&block);
            builder.GetBuilder().CreateRetVoid();
            continue;
        }
        Error(Where, "not all code paths return a value: in function {} ({})", Name, function_name);
    }

    if (verifyFunction(*function, &llvm::errs()))
    {
        function->print(llvm::errs());
        Error(Where, "failed to verify function {} ({})", Name, function_name);
    }

    builder.GetBuilder().SetInsertPoint(end_block);
}

std::ostream &NJS::FunctionStmt::Print(std::ostream &os)
{
    switch (FnID)
    {
        case FnType_Function:
            os << "function ";
            break;
        case FnType_Extern:
            os << "extern ";
            break;
        case FnType_Operator:
            os << "operator";
            break;
        default:
            break;
    }
    os << Name << "(";
    for (unsigned i = 0; i < Args.size(); ++i)
    {
        if (i > 0)
            os << ", ";
        Args[i]->Print(os);
    }
    if (VarArg)
    {
        if (!Args.empty())
            os << ", ";
        os << "...";
    }
    ResultType->Print(os << "): ");
    if (Body)
        Body->Print(os << ' ');
    return os;
}

NJS::FunctionExpr::FunctionExpr(
    SourceLocation where,
    std::vector<ParamPtr> args,
    const bool vararg,
    TypePtr result_type,
    StmtPtr body)
    : Expr(std::move(where)),
      Args(std::move(args)),
      VarArg(vararg),
      ResultType(std::move(result_type)),
      Body(std::move(body))
{
}

NJS::ValuePtr NJS::FunctionExpr::GenLLVM(Builder &builder, const TypePtr &) const
{
    static unsigned id = 0;
    const auto function_name = std::to_string(id++);

    std::vector<TypePtr> args;
    for (const auto &arg: Args)
        args.push_back(arg->Type);
    const auto type = builder.GetTypeContext().GetFunctionType(ResultType, args, VarArg);
    const auto function = llvm::Function::Create(
        type->GenFnLLVM(Where, builder),
        llvm::GlobalValue::InternalLinkage,
        builder.GetName(false, function_name),
        builder.GetModule());

    const auto end_block = builder.GetBuilder().GetInsertBlock();
    const auto entry_block = llvm::BasicBlock::Create(builder.GetContext(), "entry", function);
    builder.GetBuilder().SetInsertPoint(entry_block);

    builder.StackPush(function_name, ResultType);
    for (unsigned i = 0; i < Args.size(); ++i)
    {
        const auto &arg = Args[i];
        const auto llvm_arg = function->getArg(i);
        llvm_arg->setName(arg->Name);

        ValuePtr arg_value;
        if (arg->Type->IsRef())
            arg_value = LValue::Create(builder, arg->Type->GetElement(), llvm_arg);
        else
            arg_value = RValue::Create(builder, arg->Type, llvm_arg);
        arg->CreateVars(builder, Where, false, arg_value);
    }

    Body->GenVoidLLVM(builder);
    builder.StackPop();

    for (auto &block: *function)
    {
        if (block.getTerminator())
            continue;
        if (function->getReturnType()->isVoidTy())
        {
            builder.GetBuilder().SetInsertPoint(&block);
            builder.GetBuilder().CreateRetVoid();
            continue;
        }
        Error(Where, "not all code paths return a value: in function lambda ({})", function_name);
    }

    if (verifyFunction(*function, &llvm::errs()))
    {
        function->print(llvm::errs());
        Error(Where, "failed to verify function lambda ({})", function_name);
    }

    builder.GetBuilder().SetInsertPoint(end_block);
    return RValue::Create(builder, type, function);
}

std::ostream &NJS::FunctionExpr::Print(std::ostream &os)
{
    os << '?';
    if (!Args.empty())
    {
        os << '(';
        for (unsigned i = 0; i < Args.size(); ++i)
        {
            if (i > 0)
                os << ", ";
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
