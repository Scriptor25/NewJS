#include <newjs/builder.hpp>
#include <newjs/std.hpp>
#include <newjs/type.hpp>

std::string NJS::FunctionType::GenString(
    const ReferenceInfo &result,
    const std::vector<ReferenceInfo> &parameters,
    const bool is_var_arg)
{
    std::string dst = "(";
    for (unsigned i = 0; i < parameters.size(); ++i)
    {
        if (i > 0)
            dst += ", ";
        dst += parameters[i].GetString();
    }
    if (is_var_arg)
    {
        if (!parameters.empty())
            dst += ", ";
        dst += "...";
    }
    return dst += ") => " + result.GetString();
}

bool NJS::FunctionType::IsPrimitive() const
{
    return true;
}

bool NJS::FunctionType::IsFunction() const
{
    return true;
}

NJS::ReferenceInfo NJS::FunctionType::GetResult(const SourceLocation &) const
{
    return m_Result;
}

NJS::ReferenceInfo NJS::FunctionType::GetParameter(const SourceLocation &, const unsigned index) const
{
    return m_Parameters[index];
}

unsigned NJS::FunctionType::GetParameterCount(const SourceLocation &) const
{
    return m_Parameters.size();
}

bool NJS::FunctionType::IsVarArg(const SourceLocation &) const
{
    return m_IsVarArg;
}

bool NJS::FunctionType::TypeInfo(const SourceLocation &, Builder &builder, std::vector<llvm::Value *> &arguments) const
{
    arguments.emplace_back(builder.GetBuilder().getInt32(ID_FUNCTION));
    return false;
}

llvm::FunctionType *NJS::FunctionType::GenFnLLVM(const SourceLocation &where, const Builder &builder) const
{
    std::vector<llvm::Type *> types;
    for (const auto &parameter: m_Parameters)
        types.emplace_back(parameter.GetLLVM(where, builder));
    return llvm::FunctionType::get(m_Result.GetLLVM(where, builder), types, m_IsVarArg);
}

NJS::FunctionType::FunctionType(
    TypeContext &type_context,
    std::string string,
    ReferenceInfo result,
    std::vector<ReferenceInfo> parameters,
    const bool is_var_arg)
    : Type(type_context, std::move(string)),
      m_Result(std::move(result)),
      m_Parameters(std::move(parameters)),
      m_IsVarArg(is_var_arg)
{
}

llvm::Type *NJS::FunctionType::GenLLVM(const SourceLocation &, const Builder &builder) const
{
    return builder.GetBuilder().getPtrTy();
}
