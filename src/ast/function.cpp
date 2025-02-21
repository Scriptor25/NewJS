#include <ranges>
#include <utility>
#include <llvm/IR/Verifier.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Parameter.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::FunctionStatement::FunctionStatement(
    SourceLocation where,
    const unsigned flags,
    std::string name,
    std::vector<std::pair<ParameterPtr, ReferenceInfo>> parameters,
    const bool is_var_arg,
    ReferenceInfo result,
    StatementPtr body)
    : Statement(std::move(where)),
      Flags(flags),
      Name(std::move(name)),
      Parameters(std::move(parameters)),
      IsVarArg(is_var_arg),
      Result(std::move(result)),
      Body(std::move(body))
{
}

void NJS::FunctionStatement::GenVoidLLVM(Builder &builder) const
{
    std::string function_name;
    if (Flags & FunctionFlags_Extern)
        function_name = Name;
    else if (Flags & FunctionFlags_Operator)
    {
        if (Parameters.size() == 1)
            function_name = builder.GetName(
                Flags & FunctionFlags_Absolute,
                (IsVarArg ? std::string() : Name)
                + Parameters[0].first->Type->GetString()
                + (IsVarArg ? Name : std::string()));
        else if (Parameters.size() == 2)
            function_name = builder.GetName(
                Flags & FunctionFlags_Absolute,
                Parameters[0].first->Type->GetString() + Name + Parameters[1].first->Type->GetString());
    }
    else
        function_name = builder.GetName(Flags & FunctionFlags_Absolute, Name);

    std::vector<ReferenceInfo> parameters;
    for (const auto &info_: Parameters | std::views::values)
        parameters.emplace_back(info_);
    const auto type = builder.GetTypeContext().GetFunctionType(Result, parameters, IsVarArg);

    auto function = builder.GetModule().getFunction(function_name);
    const auto new_define = !function;

    if (new_define)
    {
        function = llvm::Function::Create(
            type->GenFnLLVM(Where, builder),
            llvm::GlobalValue::ExternalLinkage,
            function_name,
            builder.GetModule());
    }

    if (Flags & FunctionFlags_Operator)
    {
        if (Parameters.size() == 1)
            builder.DefineOperator(
                Name,
                !IsVarArg,
                Parameters[0].second,
                Result,
                function);
        else if (Parameters.size() == 2)
            builder.DefineOperator(
                Name,
                Parameters[0].second,
                Parameters[1].second,
                Result,
                function);
    }
    else
    {
        auto value = RValue::Create(builder, type, function);
        if (new_define)
            builder.DefineVariable(Where, Name) = std::move(value);
        else
        {
            auto &reference = builder.GetOrDefineVariable(Name);
            if (reference && reference->GetType() != value->GetType())
                Error(Where, "function prototype mismatch: {} != {}", reference->GetType(), value->GetType());
            reference = std::move(value);
        }
    }

    if (!Body)
        return;

    if (!function->empty())
        Error(Where, "redefining function '{}' ({})", Name, function_name);

    const auto end_block = builder.GetBuilder().GetInsertBlock();
    const auto entry_block = llvm::BasicBlock::Create(builder.GetContext(), "entry", function);
    builder.GetBuilder().SetInsertPoint(entry_block);

    builder.StackPush(Name, Result);
    for (unsigned i = 0; i < Parameters.size(); ++i)
    {
        const auto &[parameter_, info_] = Parameters[i];
        const auto argument = function->getArg(i);
        argument->setName(parameter_->Name);

        ValuePtr argument_value;
        if (info_.IsReference)
            argument_value = LValue::Create(
                builder,
                parameter_->Type,
                argument,
                info_.IsConst);
        else
            argument_value = RValue::Create(builder, parameter_->Type, argument);
        parameter_->CreateVars(
            builder,
            argument_value,
            false,
            info_.IsConst,
            info_.IsReference);
    }

    Body->GenVoidLLVM(builder);
    builder.StackPop();

    std::vector<llvm::BasicBlock *> deletable;
    for (auto &block: *function)
    {
        if (!block.hasNPredecessorsOrMore(1) && block.empty())
        {
            deletable.emplace_back(&block);
            continue;
        }
        if (block.getTerminator())
            continue;
        if (function->getReturnType()->isVoidTy())
        {
            builder.GetBuilder().SetInsertPoint(&block);
            builder.GetBuilder().CreateRetVoid();
            continue;
        }
        function->print(llvm::errs());
        Error(Where, "not all code paths return a value: in function '{}' ({})", Name, function_name);
    }

    for (const auto block: deletable)
        block->eraseFromParent();

    if (verifyFunction(*function, &llvm::errs()))
    {
        function->print(llvm::errs());
        Error(Where, "failed to verify function '{}' ({})", Name, function_name);
    }

    builder.Optimize(function);

    builder.GetBuilder().SetInsertPoint(end_block);
}

std::ostream &NJS::FunctionStatement::Print(std::ostream &stream)
{
    if (Flags & FunctionFlags_Extern)
        stream << "extern ";

    stream << "function ";

    if (Flags & FunctionFlags_Operator)
        stream << "operator";

    stream << Name << "(";
    for (unsigned i = 0; i < Parameters.size(); ++i)
    {
        if (i > 0)
            stream << ", ";
        if (Parameters[i].second.IsReference)
        {
            if (Parameters[i].second.IsConst)
                stream << "const ";
            stream << "&";
        }
        Parameters[i].first->Print(stream);
    }
    if (IsVarArg)
    {
        if (!Parameters.empty())
            stream << ", ";
        stream << "...";
    }
    Result.Print(stream << "): ");
    if (Body)
        Body->Print(stream << ' ');
    return stream;
}

NJS::FunctionExpression::FunctionExpression(
    SourceLocation where,
    std::vector<std::pair<ParameterPtr, ReferenceInfo>> parameters,
    const bool is_var_arg,
    ReferenceInfo result,
    StatementPtr body)
    : Expression(std::move(where)),
      Parameters(std::move(parameters)),
      IsVarArg(is_var_arg),
      Result(std::move(result)),
      Body(std::move(body))
{
}

NJS::ValuePtr NJS::FunctionExpression::GenLLVM(Builder &builder, const TypePtr &) const
{
    static unsigned id = 0;
    const auto function_name = std::to_string(id++);

    std::vector<ReferenceInfo> parameters;
    for (const auto &info_: Parameters | std::views::values)
        parameters.emplace_back(info_);
    const auto type = builder.GetTypeContext().GetFunctionType(Result, parameters, IsVarArg);
    const auto function = llvm::Function::Create(
        type->GenFnLLVM(Where, builder),
        llvm::GlobalValue::InternalLinkage,
        builder.GetName(false, function_name),
        builder.GetModule());

    const auto end_block = builder.GetBuilder().GetInsertBlock();
    const auto entry_block = llvm::BasicBlock::Create(builder.GetContext(), "entry", function);
    builder.GetBuilder().SetInsertPoint(entry_block);

    builder.StackPush(function_name, Result);
    for (unsigned i = 0; i < Parameters.size(); ++i)
    {
        const auto &[parameter_, info_] = Parameters[i];
        const auto argument = function->getArg(i);
        argument->setName(parameter_->Name);

        ValuePtr argument_value;
        if (info_.IsReference)
            argument_value = LValue::Create(
                builder,
                parameter_->Type,
                argument,
                info_.IsConst);
        else
            argument_value = RValue::Create(builder, parameter_->Type, argument);
        parameter_->CreateVars(
            builder,
            argument_value,
            false,
            info_.IsConst,
            info_.IsReference);
    }

    Body->GenVoidLLVM(builder);
    builder.StackPop();

    std::vector<llvm::BasicBlock *> deletable;
    for (auto &block: *function)
    {
        if (!block.hasNPredecessorsOrMore(1) && block.empty())
        {
            deletable.emplace_back(&block);
            continue;
        }
        if (block.getTerminator())
            continue;
        if (function->getReturnType()->isVoidTy())
        {
            builder.GetBuilder().SetInsertPoint(&block);
            builder.GetBuilder().CreateRetVoid();
            continue;
        }
        function->print(llvm::errs());
        Error(Where, "not all code paths return a value: in function lambda ({})", function_name);
    }

    for (const auto block: deletable)
        block->eraseFromParent();

    if (verifyFunction(*function, &llvm::errs()))
    {
        function->print(llvm::errs());
        Error(Where, "failed to verify function lambda ({})", function_name);
    }

    builder.Optimize(function);

    builder.GetBuilder().SetInsertPoint(end_block);
    return RValue::Create(builder, type, function);
}

std::ostream &NJS::FunctionExpression::Print(std::ostream &stream)
{
    stream << '?';
    if (!Parameters.empty())
    {
        stream << '(';
        for (unsigned i = 0; i < Parameters.size(); ++i)
        {
            if (i > 0)
                stream << ", ";
            if (Parameters[i].second.IsReference)
            {
                if (Parameters[i].second.IsConst)
                    stream << "const ";
                stream << "&";
            }
            Parameters[i].first->Print(stream);
        }
        if (IsVarArg)
        {
            if (!Parameters.empty())
                stream << ", ";
            stream << "...";
        }
        Result.Print(stream << "): ") << ' ';
    }
    return Body->Print(stream);
}
