#include <newjs/builder.hpp>
#include <newjs/std.hpp>
#include <newjs/type.hpp>

std::string NJS::IntegerType::GenString(const unsigned bits, const bool is_signed)
{
    return (is_signed ? 'i' : 'u') + std::to_string(bits);
}

bool NJS::IntegerType::IsPrimitive() const
{
    return true;
}

bool NJS::IntegerType::IsInteger() const
{
    return true;
}

bool NJS::IntegerType::IsSigned() const
{
    return m_IsSigned;
}

unsigned NJS::IntegerType::GetBits() const
{
    return m_Bits;
}

bool NJS::IntegerType::TypeInfo(Builder &builder, std::vector<llvm::Value *> &arguments) const
{
    arguments.emplace_back(builder.GetBuilder().getInt32(ID_INTEGER));
    arguments.emplace_back(builder.GetBuilder().getInt32(m_Bits));
    arguments.emplace_back(builder.GetBuilder().getInt32(m_IsSigned ? 1 : 0));
    return false;
}

NJS::IntegerType::IntegerType(
    TypeContext &type_context,
    std::string string,
    const unsigned bits,
    const bool is_signed)
    : Type(type_context, std::move(string)),
      m_Bits(bits),
      m_IsSigned(is_signed)
{
}

llvm::Type *NJS::IntegerType::GenLLVM(const Builder &builder) const
{
    return builder.GetBuilder().getIntNTy(m_Bits);
}
