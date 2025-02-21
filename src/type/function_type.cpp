#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Std.hpp>
#include <NJS/Type.hpp>

std::string NJS::FunctionType::GenString(
    const TypePtr &result_type,
    const std::vector<TypePtr> &parameter_types,
    const bool var_arg)
{
    std::string dst = "(";
    for (unsigned i = 0; i < parameter_types.size(); ++i)
    {
        if (i > 0)
            dst += ", ";
        dst += parameter_types[i]->GetString();
    }
    if (var_arg)
    {
        if (!parameter_types.empty())
            dst += ", ";
        dst += "...";
    }
    return dst += ") => " + result_type->GetString();
}

bool NJS::FunctionType::IsPrimitive() const
{
    return true;
}

bool NJS::FunctionType::IsFunction() const
{
    return true;
}

NJS::TypePtr NJS::FunctionType::GetResultType(const SourceLocation &) const
{
    return m_ResultType;
}

NJS::TypePtr NJS::FunctionType::GetParameterType(const SourceLocation &, const unsigned index) const
{
    return m_ParameterTypes[index];
}

unsigned NJS::FunctionType::GetParameterCount(const SourceLocation &) const
{
    return m_ParameterTypes.size();
}

bool NJS::FunctionType::IsVarArg(const SourceLocation &) const
{
    return m_VarArg;
}

void NJS::FunctionType::TypeInfo(const SourceLocation &, Builder &builder, std::vector<llvm::Value *> &args) const
{
    args.push_back(builder.GetBuilder().getInt32(ID_FUNCTION));
}

llvm::FunctionType *NJS::FunctionType::GenFnLLVM(const SourceLocation &where, const Builder &builder) const
{
    std::vector<llvm::Type *> types;
    for (const auto &arg: m_ParameterTypes)
        types.push_back(arg->GetLLVM(where, builder));
    return llvm::FunctionType::get(m_ResultType->GetLLVM(where, builder), types, m_VarArg);
}

NJS::FunctionType::FunctionType(
    TypeContext &type_context,
    std::string string,
    TypePtr result_type,
    std::vector<TypePtr> parameter_types,
    const bool var_arg)
    : Type(type_context, std::move(string)),
      m_ResultType(std::move(result_type)),
      m_ParameterTypes(std::move(parameter_types)),
      m_VarArg(var_arg)
{
}

llvm::Type *NJS::FunctionType::GenLLVM(const SourceLocation &, const Builder &builder) const
{
    return builder.GetBuilder().getPtrTy();
}

unsigned NJS::FunctionType::GenSize() const
{
    return 0;
}
