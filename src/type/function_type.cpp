#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Std.hpp>
#include <NJS/Type.hpp>

std::string NJS::FunctionType::GenString(
    const TypePtr& result,
    const std::vector<TypePtr>& args,
    const bool vararg)
{
    std::string dst = "(";
    for (unsigned i = 0; i < args.size(); ++i)
    {
        if (i > 0) dst += ", ";
        dst += args[i]->GetString();
    }
    if (vararg)
    {
        if (!args.empty()) dst += ", ";
        dst += "...";
    }
    return dst += "): " + result->GetString();
}

bool NJS::FunctionType::IsPrimitive() const
{
    return true;
}

bool NJS::FunctionType::IsFunction() const
{
    return true;
}

NJS::TypePtr NJS::FunctionType::GetResult() const
{
    return m_Result;
}

NJS::TypePtr NJS::FunctionType::Arg(const unsigned i) const
{
    return m_Args[i];
}

bool NJS::FunctionType::VarArg() const
{
    return m_VarArg;
}

void NJS::FunctionType::TypeInfo(Builder& builder, std::vector<llvm::Value*>& args) const
{
    args.push_back(builder.GetBuilder().getInt32(ID_FUNCTION));
    m_Result->TypeInfo(builder, args);
    args.push_back(builder.GetBuilder().getInt32(m_Args.size()));
    for (const auto& arg : m_Args)
        arg->TypeInfo(builder, args);
    args.push_back(builder.GetBuilder().getInt32(m_VarArg ? 1 : 0));
}

llvm::FunctionType* NJS::FunctionType::GenFnLLVM(const Builder& builder) const
{
    std::vector<llvm::Type*> types;
    for (const auto& arg : m_Args)
        types.push_back(arg->GetLLVM(builder));
    return llvm::FunctionType::get(m_Result->GetLLVM(builder), types, m_VarArg);
}

NJS::FunctionType::FunctionType(
    TypeContext& ctx,
    std::string string,
    TypePtr result,
    std::vector<TypePtr> args,
    const bool vararg)
    : Type(ctx, std::move(string)), m_Result(std::move(result)), m_Args(std::move(args)), m_VarArg(vararg)
{
}

llvm::Type* NJS::FunctionType::GenLLVM(const Builder& builder) const
{
    return builder.GetBuilder().getPtrTy();
}

unsigned NJS::FunctionType::GenSize() const
{
    return 0;
}
