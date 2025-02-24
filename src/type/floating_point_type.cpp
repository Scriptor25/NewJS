#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Std.hpp>
#include <NJS/Type.hpp>

std::string NJS::FloatingPointType::GenString(const unsigned bits)
{
    return 'f' + std::to_string(bits);
}

bool NJS::FloatingPointType::IsPrimitive() const
{
    return true;
}

bool NJS::FloatingPointType::IsFloatingPoint() const
{
    return true;
}

unsigned NJS::FloatingPointType::GetBits(const SourceLocation &where) const
{
    return m_Bits;
}

bool NJS::FloatingPointType::TypeInfo(
    const SourceLocation &,
    Builder &builder,
    std::vector<llvm::Value *> &arguments) const
{
    arguments.emplace_back(builder.GetBuilder().getInt32(ID_FLOATING_POINT));
    arguments.emplace_back(builder.GetBuilder().getInt32(m_Bits));
    return false;
}

NJS::FloatingPointType::FloatingPointType(TypeContext &type_context, std::string string, const unsigned bits)
    : Type(type_context, std::move(string)),
      m_Bits(bits)
{
}

llvm::Type *NJS::FloatingPointType::GenLLVM(const SourceLocation &where, const Builder &builder) const
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
            Error(where, "no llvm representation for floating point type {}", m_String);
    }
}
