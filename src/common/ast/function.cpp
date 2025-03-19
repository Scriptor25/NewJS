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

std::ostream &NJS::FunctionStatement::Print(std::ostream &stream) const
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
        Parameters[i]->Print(stream, true);
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

void NJS::FunctionStatement::PGenLLVM(Builder &builder, const bool is_export)
{
    const bool is_extern = Flags & FunctionFlags_Extern;
    const bool is_operator = Flags & FunctionFlags_Operator;
    const bool is_absolute = Flags & FunctionFlags_Absolute;

    std::string function_name;
    if (is_extern)
    {
        function_name = Name;
    }
    else if (is_operator)
    {
        function_name = builder.GetName(
            is_absolute,
            Builder::GetFunctionName({}, Name, Parameters, IsVarArg, is_extern, is_operator));
    }
    else
    {
        function_name = builder.GetName(is_absolute, Name);
    }

    std::vector<ReferenceInfo> parameters;
    for (const auto &parameter: Parameters)
        parameters.emplace_back(parameter->Info);
    const auto type = builder.GetTypeContext().GetFunctionType(Result, parameters, IsVarArg);

    auto function = builder.GetModule().getFunction(function_name);
    const auto new_define = !function;

    if (new_define)
    {
        function = llvm::Function::Create(
            type->GenFnLLVM(builder),
            is_export || is_extern
                ? llvm::GlobalValue::ExternalLinkage
                : llvm::GlobalValue::InternalLinkage,
            function_name,
            builder.GetModule());
    }

    if (is_operator)
    {
        if (Name == "()")
            builder.DefineOperator(
                parameters[0],
                {parameters.begin() + 1, parameters.end()},
                IsVarArg,
                Result,
                function);
        else if (Parameters.size() == 1)
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
        else
            Error("TODO");
    }
    else
    {
        auto value = RValue::Create(builder, type, function);
        if (new_define)
        {
            builder.DefineVariable(Name, true) = value;
        }
        else
        {
            auto &other = builder.GetOrDefineVariable(Name, true);
            if (other && other->GetType() != value->GetType())
                Error(Where, "function declaration mismatch, {} != {}", other->GetType(), value->GetType());
            other = std::move(value);
        }
    }

    if (!Body)
        return;

    if (!function->empty())
        Error(Where, "cannot re-define function");

    const auto insert_block = builder.GetBuilder().GetInsertBlock();
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
                parameter->Info.Type,
                argument,
                parameter->Info.IsConst);
        else
            argument_value = RValue::Create(builder, parameter->Info.Type, argument);

        parameter->CreateVars(
            builder,
            argument_value,
            false,
            false,
            parameter->Info.IsConst,
            parameter->Info.IsReference);
    }

    Body->GenLLVM(builder, false);

    builder.StackPop();

    std::vector<llvm::BasicBlock *> deletable;
    for (auto &block: *function)
    {
        if (&block != entry_block && block.hasNPredecessors(0) && block.empty())
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
        Error(Where, "not all code paths return");
    }

    for (const auto block: deletable)
        block->eraseFromParent();

    if (verifyFunction(*function, &llvm::errs()))
    {
        function->print(llvm::errs());
        Error(Where, "failed to verify function");
    }

    builder.Optimize(function);

    builder.GetBuilder().SetInsertPoint(insert_block);
}

void NJS::FunctionStatement::PGenImport(
    Builder &builder,
    const std::string &module_id,
    ValuePtr &dest_value,
    ReferenceInfo &dest_info,
    std::string &dest_name)
{
    const bool is_extern = Flags & FunctionFlags_Extern;
    const bool is_operator = Flags & FunctionFlags_Operator;

    const auto function_name = Builder::GetFunctionName(
        module_id,
        Name,
        Parameters,
        IsVarArg,
        is_extern,
        is_operator);

    std::vector<ReferenceInfo> parameters;
    for (const auto &parameter: Parameters)
        parameters.emplace_back(parameter->Info);
    const auto type = builder.GetTypeContext().GetFunctionType(Result, parameters, IsVarArg);

    const auto fn = builder.GetOrCreateFunction(
        type->GenFnLLVM(builder),
        llvm::GlobalValue::ExternalLinkage,
        function_name);

    if (is_operator)
    {
        if (Name == "()")
        {
            builder.DefineOperator(
                parameters[0],
                {parameters.begin() + 1, parameters.end()},
                IsVarArg,
                Result,
                fn);
            return;
        }
        if (Parameters.size() == 1)
        {
            builder.DefineOperator(
                Name,
                !IsVarArg,
                Parameters[0]->Info,
                Result,
                fn);
            return;
        }
        if (Parameters.size() == 2)
        {
            builder.DefineOperator(
                Name,
                Parameters[0]->Info,
                Parameters[1]->Info,
                Result,
                fn);
            return;
        }
        Error("TODO");
    }

    dest_value = RValue::Create(builder, type, fn);
    dest_info = {type, false, false};
    dest_name = Name;
}

NJS::FunctionExpression::FunctionExpression(
    SourceLocation where,
    std::vector<std::pair<ParameterPtr, ExpressionPtr>> capture_parameters,
    std::vector<ParameterPtr> parameters,
    const bool is_var_arg,
    ReferenceInfo result,
    StatementPtr body)
    : Expression(std::move(where)),
      CaptureParameters(std::move(capture_parameters)),
      Parameters(std::move(parameters)),
      IsVarArg(is_var_arg),
      Result(std::move(result)),
      Body(std::move(body))
{
}

std::ostream &NJS::FunctionExpression::Print(std::ostream &stream) const
{
    stream << '$';
    if (!CaptureParameters.empty())
    {
        stream << '[';
        for (unsigned i = 0; i < CaptureParameters.size(); ++i)
        {
            auto &[parameter, value] = CaptureParameters[i];
            if (i > 0)
                stream << ", ";
            parameter->Print(stream, true);
            stream << ": ";
            value->Print(stream);
        }
        stream << ']';
        if (Parameters.empty())
            stream << ' ';
    }
    if (!Parameters.empty())
    {
        stream << '(';
        for (unsigned i = 0; i < Parameters.size(); ++i)
        {
            if (i > 0)
                stream << ", ";
            Parameters[i]->Print(stream, true);
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

NJS::ValuePtr NJS::FunctionExpression::PGenLLVM(Builder &builder, const TypePtr &)
{
    std::vector<ReferenceInfo> parameters;
    for (const auto &parameter: Parameters)
        parameters.emplace_back(parameter->Info);
    const auto type = builder.GetTypeContext().GetFunctionType(Result, parameters, IsVarArg);

    const auto function = llvm::Function::Create(
        type->GenFnLLVM(builder),
        llvm::GlobalValue::InternalLinkage,
        builder.GetName(false, "lambda"),
        builder.GetModule());

    const auto insert_block = builder.GetBuilder().GetInsertBlock();
    const auto entry_block = llvm::BasicBlock::Create(builder.GetContext(), "entry", function);
    builder.GetBuilder().SetInsertPoint(entry_block);

    builder.StackPush("lambda", Result);
    for (unsigned i = 0; i < Parameters.size(); ++i)
    {
        const auto parameter = Parameters[i];
        const auto argument = function->getArg(i);
        argument->setName(parameter->Name);

        ValuePtr argument_value;
        if (parameter->Info.IsReference)
            argument_value = LValue::Create(
                builder,
                parameter->Info.Type,
                argument,
                parameter->Info.IsConst);
        else
            argument_value = RValue::Create(builder, parameter->Info.Type, argument);
        parameter->CreateVars(
            builder,
            argument_value,
            false,
            false,
            parameter->Info.IsConst,
            parameter->Info.IsReference);
    }

    Body->GenLLVM(builder, false);

    builder.StackPop();

    std::vector<llvm::BasicBlock *> deletable;
    for (auto &block: *function)
    {
        if (&block != entry_block && block.hasNPredecessors(0) && block.empty())
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
        Error(Where, "not all code paths return");
    }

    for (const auto block: deletable)
        block->eraseFromParent();

    if (verifyFunction(*function, &llvm::errs()))
    {
        function->print(llvm::errs());
        Error(Where, "failed to verify function");
    }

    builder.Optimize(function);

    builder.GetBuilder().SetInsertPoint(insert_block);
    return RValue::Create(builder, type, function);
}
