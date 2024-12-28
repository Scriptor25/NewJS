#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Std.hpp>
#include <NJS/Type.hpp>

std::string NJS::IntType::GenString(const unsigned bits, const bool is_signed)
{
    return (is_signed ? 'i' : 'u') + std::to_string(bits);
}

bool NJS::IntType::IsPrimitive() const
{
    return true;
}

bool NJS::IntType::IsInt() const
{
    return true;
}

bool NJS::IntType::IsSigned() const
{
    return m_IsSigned;
}

unsigned NJS::IntType::GetBits() const
{
    return m_Bits;
}

void NJS::IntType::TypeInfo(Builder& builder, std::vector<llvm::Value*>& args) const
{
    args.push_back(builder.GetBuilder().getInt32(ID_INT));
    args.push_back(builder.GetBuilder().getInt32(m_Bits));
    args.push_back(builder.GetBuilder().getInt32(m_IsSigned ? 1 : 0));
}

NJS::IntType::IntType(TypeContext& ctx, std::string string, const unsigned bits, const bool is_signed)
    : Type(ctx, std::move(string)), m_Bits(bits), m_IsSigned(is_signed)
{
}

llvm::Type* NJS::IntType::GenLLVM(const SourceLocation&, const Builder& builder) const
{
    return builder.GetBuilder().getIntNTy(m_Bits);
}

unsigned NJS::IntType::GenSize() const
{
    auto bits = m_Bits;
    if (const auto rem = bits % 8)
        bits += bits - rem;
    return bits / 8;
}
