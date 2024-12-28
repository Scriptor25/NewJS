#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Std.hpp>
#include <NJS/Type.hpp>

std::string NJS::FPType::GenString(const unsigned bits)
{
    return 'f' + std::to_string(bits);
}

bool NJS::FPType::IsPrimitive() const
{
    return true;
}

bool NJS::FPType::IsFP() const
{
    return true;
}

unsigned NJS::FPType::GetBits() const
{
    return m_Bits;
}

void NJS::FPType::TypeInfo(Builder& builder, std::vector<llvm::Value*>& args) const
{
    args.push_back(builder.GetBuilder().getInt32(ID_FP));
    args.push_back(builder.GetBuilder().getInt32(m_Bits));
}

NJS::FPType::FPType(TypeContext& ctx, std::string string, const unsigned bits)
    : Type(ctx, std::move(string)), m_Bits(bits)
{
}

llvm::Type* NJS::FPType::GenLLVM(const SourceLocation&, const Builder& builder) const
{
    switch (m_Bits)
    {
    case 16: return builder.GetBuilder().getHalfTy();
    case 32: return builder.GetBuilder().getFloatTy();
    case 64: return builder.GetBuilder().getDoubleTy();
    default: Error("no llvm representation for floating point type {}", m_String);
    }
}

unsigned NJS::FPType::GenSize() const
{
    return m_Bits / 8;
}
