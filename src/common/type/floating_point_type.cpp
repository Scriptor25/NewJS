#include <newjs/builder.hpp>
#include <newjs/std.hpp>
#include <newjs/type.hpp>

std::string NJS::FloatingPointType::GenString(const unsigned bits)
{
    return 'f' + std::to_string(bits);
}

unsigned NJS::FloatingPointType::GenHash(const unsigned bits)
{
    return std::hash<unsigned>()(bits) ^ 0x03;
}

bool NJS::FloatingPointType::IsPrimitive() const
{
    return true;
}

bool NJS::FloatingPointType::IsFloatingPoint() const
{
    return true;
}

unsigned NJS::FloatingPointType::GetBits() const
{
    return m_Bits;
}

bool NJS::FloatingPointType::TypeInfo(
    Builder &builder,
    std::vector<llvm::Value *> &arguments) const
{
    arguments.emplace_back(builder.GetBuilder().getInt32(ID_FLOATING_POINT));
    arguments.emplace_back(builder.GetBuilder().getInt32(m_Bits));
    return false;
}

std::ostream &NJS::FloatingPointType::Print(std::ostream &stream) const
{
    return stream << m_String;
}

NJS::FloatingPointType::FloatingPointType(
    TypeContext &type_context,
    const unsigned hash,
    std::string string,
    const unsigned bits)
    : Type(type_context, hash, std::move(string)),
      m_Bits(bits)
{
}

llvm::Type *NJS::FloatingPointType::GenLLVM(const Builder &builder) const
{
    switch (m_Bits)
    {
        case 16:
            return builder.GetBuilder().getHalfTy();
        case 32:
            return builder.GetBuilder().getFloatTy();
        case 64:
            return builder.GetBuilder().getDoubleTy();
        default:
            Error("floating point type with {} bits does not have a llvm representation", m_Bits);
    }
}
