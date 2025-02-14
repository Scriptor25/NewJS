#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>

NJS::TypePtr NJS::GetHigherOrderOf(TypeContext &type_context, const TypePtr &type_a, const TypePtr &type_b)
{
    if (type_a == type_b)
        return type_a;

    if (type_a->IsInteger())
    {
        if (type_b->IsInteger())
            return type_context.GetIntegerType(
                std::max(type_a->GetBits(), type_b->GetBits()),
                type_a->IsSigned() || type_b->IsSigned());
        if (type_b->IsFloatingPoint())
            return type_context.GetFloatingPointType(std::max(type_a->GetBits(), type_b->GetBits()));
        if (type_b->IsPointer())
            return type_b;
    }

    if (type_a->IsFloatingPoint())
    {
        if (type_b->IsInteger() || type_b->IsFloatingPoint())
            return type_context.GetFloatingPointType(std::max(type_a->GetBits(), type_b->GetBits()));
    }

    if (type_a->IsPointer())
    {
        if (type_b->IsInteger())
            return type_a;
    }

    return {};
}

unsigned NJS::GetAssignmentError(const TypePtr &parameter_type, const TypePtr &argument_type)
{
    if (argument_type == parameter_type)
        return 0u;

    if (argument_type->IsInteger())
    {
        if (parameter_type->IsInteger())
        {
            const auto same_signage = argument_type->IsSigned() == parameter_type->IsSigned();
            const auto bits_difference = static_cast<int>(argument_type->GetBits())
                                         - static_cast<int>(parameter_type->GetBits());
            return (same_signage ? 1u : 0u) + abs(bits_difference);
        }
        if (parameter_type->IsFloatingPoint())
        {
            const auto bits_difference = static_cast<int>(argument_type->GetBits())
                                         - static_cast<int>(parameter_type->GetBits());
            return 3u * abs(bits_difference);
        }
        if (parameter_type->IsPointer())
        {
            const auto same_signage = !argument_type->IsSigned();
            const auto bits_difference = static_cast<int>(argument_type->GetBits()) - 64;
            return 2u * ((same_signage ? 1u : 0u) + abs(bits_difference));
        }
    }

    if (argument_type->IsFloatingPoint())
    {
        if (parameter_type->IsFloatingPoint())
        {
            const auto bits_difference = static_cast<int>(argument_type->GetBits())
                                         - static_cast<int>(parameter_type->GetBits());
            return abs(bits_difference);
        }
        if (parameter_type->IsInteger())
        {
            const auto bits_difference = static_cast<int>(argument_type->GetBits())
                                         - static_cast<int>(parameter_type->GetBits());
            return 2u * abs(bits_difference);
        }
    }

    if (argument_type->IsPointer())
    {
        if (parameter_type->IsPointer())
        {
            const auto parameter_is_void = parameter_type->GetElement()->IsVoid();
            const auto argument_is_void = argument_type->GetElement()->IsVoid();
            return parameter_is_void || argument_is_void ? 1u : ~0u;
        }
    }

    return ~0u;
}

std::ostream &NJS::Type::Print(std::ostream &stream) const
{
    return stream << m_String;
}

std::string NJS::Type::GetString() const
{
    return m_String;
}

unsigned NJS::Type::GetSize()
{
    if (m_Size != ~0u)
        return m_Size;
    return m_Size = GenSize();
}

bool NJS::Type::IsPrimitive() const
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

bool NJS::Type::IsReference() const
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

bool NJS::Type::IsSigned() const
{
    Error("type {} does not support 'IsSigned'", m_String);
}

unsigned NJS::Type::GetBits() const
{
    Error("type {} does not support 'GetBits'", m_String);
}

NJS::TypePtr NJS::Type::GetElement() const
{
    Error("type {} does not support 'GetElement'", m_String);
}

NJS::TypePtr NJS::Type::GetElement(unsigned) const
{
    Error("type {} does not support 'GetElement'", m_String);
}

NJS::MemberT NJS::Type::GetMember(const std::string_view &) const
{
    Error("type {} does not support 'GetMember'", m_String);
}

NJS::TypePtr NJS::Type::GetResultType() const
{
    Error("type {} does not support 'GetResultType'", m_String);
}

NJS::Type::Type(TypeContext &type_context, std::string_view string)
    : m_TypeContext(type_context),
      m_String(std::move(string)),
      m_LLVM(nullptr),
      m_Size(~0u)
{
}
