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

unsigned NJS::FunctionType::GenHash(
    const ReferenceInfo &result,
    const std::vector<ReferenceInfo> &parameters,
    const bool is_var_arg)
{
    auto hash = CombineHashes(result.GetHash(), 0x08);
    for (unsigned i = 0; i < parameters.size(); ++i)
        hash = CombineHashes(hash, parameters[i].GetHash());
    return CombineHashes(hash, std::hash<bool>()(is_var_arg));
}

bool NJS::FunctionType::IsPrimitive() const
{
    return true;
}

bool NJS::FunctionType::IsFunction() const
{
    return true;
}

NJS::ReferenceInfo NJS::FunctionType::GetResult() const
{
    return m_Result;
}

NJS::ReferenceInfo NJS::FunctionType::GetParameter(const unsigned index) const
{
    return m_Parameters[index];
}

unsigned NJS::FunctionType::GetParameterCount() const
{
    return m_Parameters.size();
}

bool NJS::FunctionType::IsVarArg() const
{
    return m_IsVarArg;
}

bool NJS::FunctionType::TypeInfo(Builder &builder, std::vector<llvm::Value *> &arguments) const
{
    arguments.emplace_back(builder.GetBuilder().getInt32(ID_FUNCTION));
    return false;
}

std::ostream &NJS::FunctionType::Print(std::ostream &stream) const
{
    stream << "(";
    for (unsigned i = 0; i < m_Parameters.size(); ++i)
    {
        if (i > 0)
            stream << ", ";
        m_Parameters[i].Print(stream);
    }
    if (m_IsVarArg)
    {
        if (!m_Parameters.empty())
            stream << ", ";
        stream << "...";
    }
    return m_Result.Print(stream << ") => ");
}

llvm::FunctionType *NJS::FunctionType::GenFnLLVM(const Builder &builder) const
{
    std::vector<llvm::Type *> types;
    for (const auto &parameter: m_Parameters)
        types.emplace_back(parameter.GetLLVM(builder));
    return llvm::FunctionType::get(m_Result.GetLLVM(builder), types, m_IsVarArg);
}

NJS::FunctionType::FunctionType(
    TypeContext &type_context,
    const unsigned hash,
    std::string string,
    ReferenceInfo result,
    std::vector<ReferenceInfo> parameters,
    const bool is_var_arg)
    : Type(type_context, hash, std::move(string)),
      m_Result(std::move(result)),
      m_Parameters(std::move(parameters)),
      m_IsVarArg(is_var_arg)
{
}

llvm::Type *NJS::FunctionType::GenLLVM(const Builder &builder) const
{
    return builder.GetBuilder().getPtrTy();
}
