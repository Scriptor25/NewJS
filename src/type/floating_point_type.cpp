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

unsigned NJS::FloatingPointType::GetBits() const
{
    return m_Bits;
}

void NJS::FloatingPointType::TypeInfo(const SourceLocation &, Builder &builder, std::vector<llvm::Value *> &args) const
{
    args.push_back(builder.GetBuilder().getInt32(ID_FP));
    args.push_back(builder.GetBuilder().getInt32(m_Bits));
}

NJS::FloatingPointType::FloatingPointType(TypeContext &type_context, std::string_view string, const unsigned bits)
    : Type(type_context, std::move(string)),
      m_Bits(bits)
{
}

llvm::Type *NJS::FloatingPointType::GenLLVM(const SourceLocation &, const Builder &builder) const
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
            Error("no llvm representation for floating point type {}", m_String);
    }
}

unsigned NJS::FloatingPointType::GenSize() const
{
    return m_Bits / 8;
}
