#include <llvm/IR/DerivedTypes.h>
#include <NJS/Builder.hpp>
#include <NJS/Std.hpp>
#include <NJS/Type.hpp>

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

NJS::FunctionType::FunctionType(
    TypeContext& ctx,
    std::vector<TypePtr> param_types,
    TypePtr result_type,
    const bool vararg)
    : Type(ctx, GenString(param_types, result_type, vararg)),
      ParamTypes(std::move(param_types)),
      ResultType(std::move(result_type)),
      VarArg(vararg)
{
}

bool NJS::FunctionType::IsFunction() const
{
    return true;
}

NJS::TypePtr NJS::FunctionType::Result() const
{
    return ResultType;
}

void NJS::FunctionType::TypeInfo(Builder& builder, std::vector<llvm::Value*>& args) const
{
    args.push_back(builder.GetBuilder().getInt32(ID_FUNCTION));
    ResultType->TypeInfo(builder, args);
    args.push_back(builder.GetBuilder().getInt64(ParamTypes.size()));
    for (const auto& param : ParamTypes)
        param->TypeInfo(builder, args);
    args.push_back(builder.GetBuilder().getInt32(VarArg));
}

size_t NJS::FunctionType::Bytes() const
{
    return 8;
}

llvm::Type* NJS::FunctionType::GenLLVM(Builder& builder) const
{
    return builder.GetBuilder().getPtrTy();
}

llvm::FunctionType* NJS::FunctionType::GenFnLLVM(Builder& builder) const
{
    const auto result = ResultType->GenLLVM(builder);
    std::vector<llvm::Type*> params;
    for (const auto& type : ParamTypes)
        params.push_back(type->GenLLVM(builder));
    return llvm::FunctionType::get(result, params, VarArg);
}
