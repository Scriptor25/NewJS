#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Std.hpp>
#include <NJS/Type.hpp>

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

bool NJS::IntegerType::IsSigned(const SourceLocation &where) const
{
    return m_IsSigned;
}

unsigned NJS::IntegerType::GetBits(const SourceLocation &where) const
{
    return m_Bits;
}

void NJS::IntegerType::TypeInfo(const SourceLocation &, Builder &builder, std::vector<llvm::Value *> &args) const
{
    args.push_back(builder.GetBuilder().getInt32(ID_INTEGER));
    args.push_back(builder.GetBuilder().getInt32(m_Bits));
    args.push_back(builder.GetBuilder().getInt32(m_IsSigned ? 1 : 0));
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

llvm::Type *NJS::IntegerType::GenLLVM(const SourceLocation &, const Builder &builder) const
{
    return builder.GetBuilder().getIntNTy(m_Bits);
}

unsigned NJS::IntegerType::GenSize() const
{
    auto bits = m_Bits;
    if (const auto rem = bits % 8)
        bits += bits - rem;
    return bits / 8;
}
