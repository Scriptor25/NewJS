#include <llvm/IR/DerivedTypes.h>
#include <NJS/Type.hpp>

#include "NJS/Builder.hpp"

std::string NJS::FunctionType::GenString(
    const std::vector<TypePtr>& param_types,
    const TypePtr& result_type,
    const bool vararg)
{
    std::string str = "(";
    bool first = true;
    for (const auto& type : param_types)
    {
        if (first) first = false;
        else str += ", ";
        str += type->String;
    }
    if (vararg)
    {
        if (!first) str += ", ";
        str += "...";
    }
    return str + ") => " + result_type->String;
}

NJS::FunctionType::FunctionType(std::vector<TypePtr> param_types, TypePtr result_type, const bool vararg)
    : Type(GenString(param_types, result_type, vararg)),
      ParamTypes(std::move(param_types)),
      ResultType(std::move(result_type)),
      VarArg(vararg)
{
}

bool NJS::FunctionType::IsComplex()
{
    return true;
}

NJS::TypePtr NJS::FunctionType::Result()
{
    return ResultType;
}

llvm::Type* NJS::FunctionType::GenLLVM(Builder& builder)
{
    const auto result = ResultType->IsComplex()
                            ? builder.LLVMBuilder().getPtrTy()
                            : ResultType->GenLLVM(builder);
    std::vector<llvm::Type*> params(ParamTypes.size());
    for (size_t i = 0; i < ParamTypes.size(); ++i)
    {
        const auto type = ParamTypes[i];
        params[i] = type->IsComplex()
                        ? builder.LLVMBuilder().getPtrTy()
                        : type->GenLLVM(builder);
    }
    return llvm::FunctionType::get(result, params, VarArg);
}
