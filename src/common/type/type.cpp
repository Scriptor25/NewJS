#include <iostream>
#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>

NJS::TypePtr NJS::GetHigherOrderOf(
    TypeContext &type_context,
    const TypePtr &type_a,
    const TypePtr &type_b)
{
    if (type_a == type_b)
        return type_a;

    if (type_a->IsInteger())
    {
        if (type_b->IsInteger())
            return type_context.GetIntegerType(
                std::max(
                    Type::As<IntegerType>(type_a)->GetBits(),
                    Type::As<IntegerType>(type_b)->GetBits()),
                Type::As<IntegerType>(type_a)->IsSigned() || Type::As<IntegerType>(type_b)->IsSigned());
        if (type_b->IsFloatingPoint())
            return type_context.GetFloatingPointType(
                std::max(
                    Type::As<IntegerType>(type_a)->GetBits(),
                    Type::As<FloatingPointType>(type_b)->GetBits()));
        if (type_b->IsPointer())
            return type_b;
    }

    if (type_a->IsFloatingPoint())
    {
        if (type_b->IsInteger())
            return type_context.GetFloatingPointType(
                std::max(
                    Type::As<FloatingPointType>(type_a)->GetBits(),
                    Type::As<IntegerType>(type_b)->GetBits()));
        if (type_b->IsFloatingPoint())
            return type_context.GetFloatingPointType(
                std::max(
                    Type::As<FloatingPointType>(type_a)->GetBits(),
                    Type::As<FloatingPointType>(type_b)->GetBits()));
    }

    if (type_a->IsPointer())
    {
        if (type_b->IsInteger())
            return type_a;
    }

    Error("cannot determine higher order type of {} and {}", type_a, type_b);
}

std::ostream &NJS::Type::Print(std::ostream &stream) const
{
    return stream << m_String;
}

std::string NJS::Type::GetString() const
{
    return m_String;
}

unsigned NJS::Type::GetSize(const Builder &builder)
{
    if (m_Size != ~0u)
        return m_Size;
    if (IsIncomplete())
        return m_Size = 0;
    const auto type = GetLLVM(builder);
    return m_Size = builder.GetModule().getDataLayout().getTypeAllocSize(type);
}

bool NJS::Type::IsIntegerLike() const
{
    return IsInteger() || IsPointer() || IsFunction();
}

bool NJS::Type::IsBoolean() const
{
    return IsInteger() && dynamic_cast<const IntegerType *>(this)->GetBits() == 1;
}

bool NJS::Type::IsPrimitive() const
{
    return false;
}

bool NJS::Type::IsIncomplete() const
{
    return false;
}

bool NJS::Type::IsVoid() const
{
    return false;
}

bool NJS::Type::IsInteger() const
{
    return false;
}

bool NJS::Type::IsFloatingPoint() const
{
    return false;
}

bool NJS::Type::IsPointer() const
{
    return false;
}

bool NJS::Type::IsArray() const
{
    return false;
}

bool NJS::Type::IsStruct() const
{
    return false;
}

bool NJS::Type::IsTuple() const
{
    return false;
}

bool NJS::Type::IsFunction() const
{
    return false;
}

NJS::Type::Type(TypeContext &type_context, std::string string)
    : m_TypeContext(type_context),
      m_String(std::move(string)),
      m_LLVM(nullptr),
      m_Size(~0u)
{
}
