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
    std::vector<ParameterPtr> parameters,
    const bool is_var_arg,
    TypePtr result_type,
    StatementPtr body)
    : Statement(std::move(where)),
      Flags(flags),
      Name(std::move(name)),
      Parameters(std::move(parameters)),
      IsVarArg(is_var_arg),
      ResultType(std::move(result_type)),
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
                Parameters[0]->Type->GetString() + Name);
        else if (Parameters.size() == 2)
            function_name = builder.GetName(
                Flags & FunctionFlags_Absolute,
                Parameters[0]->Type->GetString() + Name + Parameters[1]->Type->GetString());
    }
    else
        function_name = builder.GetName(Flags & FunctionFlags_Absolute, Name);

    auto function = builder.GetModule().getFunction(function_name);
    if (!function)
    {
        std::vector<TypePtr> parameter_types;
        for (const auto &parameter: Parameters)
            parameter_types.push_back(parameter->Type);
        const auto type = builder.GetTypeContext().GetFunctionType(ResultType, parameter_types, IsVarArg);
        function = llvm::Function::Create(
            type->GenFnLLVM(Where, builder),
            llvm::GlobalValue::ExternalLinkage,
            function_name,
            builder.GetModule());

        if (Flags & FunctionFlags_Operator)
        {
            if (Parameters.size() == 1)
                builder.DefineOperator(Name, Parameters[0]->Type, ResultType, function);
            else if (Parameters.size() == 2)
                builder.DefineOperator(Name, Parameters[0]->Type, Parameters[1]->Type, ResultType, function);
        }
        else
            builder.DefineVariable(Where, Name) = RValue::Create(builder, type, function);
    }

    if (!Body)
        return;

    if (!function->empty())
        Error(Where, "redefining function {} ({})", Name, function_name);

    const auto end_block = builder.GetBuilder().GetInsertBlock();
    const auto entry_block = llvm::BasicBlock::Create(builder.GetContext(), "entry", function);
    builder.GetBuilder().SetInsertPoint(entry_block);

    builder.StackPush(Name, ResultType);
    for (unsigned i = 0; i < Parameters.size(); ++i)
    {
        const auto &parameter = Parameters[i];
        const auto argument = function->getArg(i);
        argument->setName(parameter->Name);

        const auto parameter_type = parameter->Type;

        ValuePtr argument_value;
        if (parameter_type->IsReference())
            argument_value = LValue::Create(
                builder,
                parameter_type->GetElement(),
                argument,
                parameter_type->IsMutable());
        else
            argument_value = RValue::Create(builder, parameter_type, argument);
        parameter->CreateVars(builder, Where, argument_value, ParameterFlags_None);
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
    ResultType->Print(stream << "): ");
    if (Body)
        Body->Print(stream << ' ');
    return stream;
}

NJS::FunctionExpression::FunctionExpression(
    SourceLocation where,
    std::vector<ParameterPtr> parameters,
    const bool is_var_arg,
    TypePtr result_type,
    StatementPtr body)
    : Expression(std::move(where)),
      Parameters(std::move(parameters)),
      IsVarArg(is_var_arg),
      ResultType(std::move(result_type)),
      Body(std::move(body))
{
}

NJS::ValuePtr NJS::FunctionExpression::GenLLVM(Builder &builder, const TypePtr &) const
{
    static unsigned id = 0;
    const auto function_name = std::to_string(id++);

    std::vector<TypePtr> parameter_types;
    for (const auto &parameter: Parameters)
        parameter_types.push_back(parameter->Type);
    const auto type = builder.GetTypeContext().GetFunctionType(ResultType, parameter_types, IsVarArg);
    const auto function = llvm::Function::Create(
        type->GenFnLLVM(Where, builder),
        llvm::GlobalValue::InternalLinkage,
        builder.GetName(false, function_name),
        builder.GetModule());

    const auto end_block = builder.GetBuilder().GetInsertBlock();
    const auto entry_block = llvm::BasicBlock::Create(builder.GetContext(), "entry", function);
    builder.GetBuilder().SetInsertPoint(entry_block);

    builder.StackPush(function_name, ResultType);
    for (unsigned i = 0; i < Parameters.size(); ++i)
    {
        auto &parameter = Parameters[i];
        const auto argument = function->getArg(i);
        argument->setName(parameter->Name);

        const auto parameter_type = parameter->Type;

        ValuePtr argument_value;
        if (parameter_type->IsReference())
            argument_value = LValue::Create(
                builder,
                parameter_type->GetElement(),
                argument,
                parameter_type->IsMutable());
        else
            argument_value = RValue::Create(builder, parameter_type, argument);
        parameter->CreateVars(builder, Where, argument_value, ParameterFlags_None);
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
        ResultType->Print(stream << "): ") << ' ';
    }
    return Body->Print(stream);
}
