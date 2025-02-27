#include <ranges>
#include <utility>
#include <llvm/IR/Verifier.h>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/error.hpp>
#include <newjs/parameter.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

NJS::FunctionStatement::FunctionStatement(
    SourceLocation where,
    const unsigned flags,
    std::string name,
    std::vector<ParameterPtr> parameters,
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

NJS::ValuePtr NJS::FunctionStatement::GenLLVM(Builder &builder, ErrorInfo &error) const
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
                + Parameters[0]->Info.GetString()
                + (IsVarArg ? Name : std::string()));
        else if (Parameters.size() == 2)
            function_name = builder.GetName(
                Flags & FunctionFlags_Absolute,
                Parameters[0]->Info.GetString() + Name + Parameters[1]->Info.GetString());
    }
    else
        function_name = builder.GetName(Flags & FunctionFlags_Absolute, Name);

    std::vector<ReferenceInfo> parameters;
    for (const auto &parameter: Parameters)
        parameters.emplace_back(parameter->Info);
    const auto type = builder.GetTypeContext().GetFunctionType(Result, parameters, IsVarArg);

    auto function = builder.GetModule().getFunction(function_name);
    const auto new_define = !function;

    if (new_define)
    {
        function = llvm::Function::Create(
            type->GenFnLLVM(Where, builder),
            (Flags & (FunctionFlags_Export | FunctionFlags_Extern))
                ? llvm::GlobalValue::ExternalLinkage
                : llvm::GlobalValue::InternalLinkage,
            function_name,
            builder.GetModule());
    }

    if (Flags & FunctionFlags_Operator)
    {
        if (Parameters.size() == 1)
            builder.DefineOperator(
                Name,
                !IsVarArg,
                Parameters[0]->Info,
                Result,
                function);
        else if (Parameters.size() == 2)
            builder.DefineOperator(
                Name,
                Parameters[0]->Info,
                Parameters[1]->Info,
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
                Error(
                    Where,
                    "function prototype mismatch: {} != {}",
                    reference->GetType(),
                    value->GetType());
            reference = std::move(value);
        }
    }

    if (!Body)
        return {};

    if (!function->empty())
        Error(Where, "redefining function '{}' ({})", Name, function_name);

    const auto end_block = builder.GetBuilder().GetInsertBlock();
    const auto entry_block = llvm::BasicBlock::Create(builder.GetContext(), "entry", function);
    builder.GetBuilder().SetInsertPoint(entry_block);

    builder.StackPush(Name, Result);
    for (unsigned i = 0; i < Parameters.size(); ++i)
    {
        const auto &parameter = Parameters[i];
        const auto argument = function->getArg(i);
        argument->setName(parameter->Name);

        ValuePtr argument_value;
        if (parameter->Info.IsReference)
            argument_value = LValue::Create(
                builder,
                parameter->Type,
                argument,
                parameter->Info.IsConst);
        else
            argument_value = RValue::Create(builder, parameter->Type, argument);
        parameter->CreateVars(
            builder,
            argument_value,
            false,
            parameter->Info.IsConst,
            parameter->Info.IsReference,
            error);
    }

    Body->GenLLVM(builder, error);
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

    return {};
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
        Parameters[i]->Print(stream);
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
    std::vector<ParameterPtr> parameters,
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

NJS::ValuePtr NJS::FunctionExpression::GenLLVM(Builder &builder, ErrorInfo &error, const TypePtr &) const
{
    static unsigned id = 0;
    const auto function_name = std::to_string(id++);

    std::vector<ReferenceInfo> parameters;
    for (const auto &parameter: Parameters)
        parameters.emplace_back(parameter->Info);
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
        const auto parameter = Parameters[i];
        const auto argument = function->getArg(i);
        argument->setName(parameter->Name);

        ValuePtr argument_value;
        if (parameter->Info.IsReference)
            argument_value = LValue::Create(
                builder,
                parameter->Type,
                argument,
                parameter->Info.IsConst);
        else
            argument_value = RValue::Create(builder, parameter->Type, argument);
        parameter->CreateVars(
            builder,
            argument_value,
            false,
            parameter->Info.IsConst,
            parameter->Info.IsReference,
            error);
    }

    Body->GenLLVM(builder, error);
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
            Parameters[i]->Print(stream);
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
