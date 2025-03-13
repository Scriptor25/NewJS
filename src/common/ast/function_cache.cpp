#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/parameter.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

NJS::FunctionCacheExpression::FunctionCacheExpression(
    SourceLocation where,
    std::string name,
    std::vector<ParameterPtr> parameters,
    const bool is_var_arg,
    ReferenceInfo result,
    StatementPtr body)
    : Expression(std::move(where)),
      Name(std::move(name)),
      Parameters(std::move(parameters)),
      IsVarArg(is_var_arg),
      Result(std::move(result)),
      Body(std::move(body))
{
}

std::ostream &NJS::FunctionCacheExpression::Print(std::ostream &stream) const
{
    return stream << "function cache";
}

NJS::ValuePtr NJS::FunctionCacheExpression::PGenLLVM(Builder &builder, const TypePtr &expected_type)
{
    if (Cache)
        return Cache;

    const auto function_name = builder.GetName(false, Name);

    std::vector<ReferenceInfo> parameters;
    for (const auto &parameter: Parameters)
        parameters.emplace_back(parameter->Info);
    const auto type = builder.GetTypeContext().GetFunctionType(Result, parameters, IsVarArg);
    auto function = builder.GetOrCreateFunction(
        type->GenFnLLVM(builder),
        llvm::GlobalValue::ExternalLinkage,
        function_name);

    if (!Body)
        return Cache = RValue::Create(builder, type, function);

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

    Body->GenLLVM(builder);

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

    return Cache = RValue::Create(builder, type, function);
}
