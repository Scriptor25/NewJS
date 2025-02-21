#include <NJS/Error.hpp>
#include <NJS/SourceLocation.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>

NJS::TypePtr NJS::GetHigherOrderOf(
    const SourceLocation &where,
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
                std::max(type_a->GetBits(where), type_b->GetBits(where)),
                type_a->IsSigned(where) || type_b->IsSigned(where));
        if (type_b->IsFloatingPoint())
            return type_context.GetFloatingPointType(std::max(type_a->GetBits(where), type_b->GetBits(where)));
        if (type_b->IsPointer())
            return type_b;
    }

    if (type_a->IsFloatingPoint())
    {
        if (type_b->IsInteger() || type_b->IsFloatingPoint())
            return type_context.GetFloatingPointType(std::max(type_a->GetBits(where), type_b->GetBits(where)));
    }

    if (type_a->IsPointer())
    {
        if (type_b->IsInteger())
            return type_a;
    }

    return {};
}

bool NJS::operator==(const TypePtr &a, const TypePtr &b)
{
    if (a.get() == b.get())
        return true;
    if (a->IsIncomplete() || b->IsIncomplete())
        return true;
    if (a->GetString() == b->GetString())
        return true;
    if (a->IsVoid() && b->IsVoid())
        return true;

    if (a->IsPrimitive() != b->IsPrimitive())
        return false;
    if (a->IsVoid() != b->IsVoid())
        return false;
    if (a->IsInteger() != b->IsInteger())
        return false;
    if (a->IsFloatingPoint() != b->IsFloatingPoint())
        return false;
    if (a->IsPointer() != b->IsPointer())
        return false;
    if (a->IsReference() != b->IsReference())
        return false;
    if (a->IsArray() != b->IsArray())
        return false;
    if (a->IsStruct() != b->IsStruct())
        return false;
    if (a->IsTuple() != b->IsTuple())
        return false;
    if (a->IsFunction() != b->IsFunction())
        return false;

    if (a->IsInteger())
        return a->IsSigned(SourceLocation()) == b->IsSigned(SourceLocation())
               && a->GetBits(SourceLocation()) == b->GetBits(SourceLocation());

    if (a->IsFloatingPoint())
        return a->GetBits(SourceLocation()) == b->GetBits(SourceLocation());

    if (a->IsPointer() || a->IsReference())
        return a->GetElement(SourceLocation()) == b->GetElement(SourceLocation());

    if (a->IsArray())
        return a->GetElement(SourceLocation()) == b->GetElement(SourceLocation())
               && a->GetElementCount(SourceLocation()) == b->GetElementCount(SourceLocation());

    if (a->IsStruct())
    {
        const auto count = a->GetElementCount(SourceLocation());
        if (count != b->GetElementCount(SourceLocation()))
            return false;
        for (unsigned i = 0; i < count; ++i)
        {
            const auto [a_index_, a_name_, a_type_] = a->GetMember(SourceLocation(), i);
            const auto [b_index_, b_name_, b_type_] = b->GetMember(SourceLocation(), i);
            if (a_name_ != b_name_)
                return false;
            if (a_type_ != b_type_)
                return false;
        }
        return true;
    }

    if (a->IsTuple())
    {
        const auto count = a->GetElementCount(SourceLocation());
        if (count != b->GetElementCount(SourceLocation()))
            return false;
        for (unsigned i = 0; i < count; ++i)
            if (a->GetElement(SourceLocation(), i) != b->GetElement(SourceLocation(), i))
                return false;
        return true;
    }

    if (a->IsFunction())
    {
        if (a->GetResultType(SourceLocation()) != b->GetResultType(SourceLocation()))
            return false;
        if (a->IsVarArg(SourceLocation()) != b->IsVarArg(SourceLocation()))
            return false;
        const auto count = a->GetParameterCount(SourceLocation());
        if (count != b->GetParameterCount(SourceLocation()))
            return false;
        for (unsigned i = 0; i < count; ++i)
            if (a->GetParameterType(SourceLocation(), i) != b->GetParameterType(SourceLocation(), i))
                return false;
        return true;
    }

    Error("'i dont even know who you are' (types {} and {})", a, b);
}

bool NJS::operator!=(const TypePtr &a, const TypePtr &b)
{
    return !(a == b);
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

bool NJS::Type::IsSigned(const SourceLocation &where) const
{
    Error(where, "type {} does not support 'IsSigned'", m_String);
}

unsigned NJS::Type::GetBits(const SourceLocation &where) const
{
    Error(where, "type {} does not support 'GetBits'", m_String);
}

NJS::TypePtr NJS::Type::GetElement(const SourceLocation &where) const
{
    Error(where, "type {} does not support 'GetElement'", m_String);
}

NJS::TypePtr NJS::Type::GetElement(const SourceLocation &where, unsigned) const
{
    Error(where, "type {} does not support 'GetElement'", m_String);
}

unsigned NJS::Type::GetElementCount(const SourceLocation &where) const
{
    Error(where, "type {} does not support 'GetElementCount'", m_String);
}

NJS::MemberInfo NJS::Type::GetMember(const SourceLocation &where, const std::string &) const
{
    Error(where, "type {} does not support 'GetMember'", m_String);
}

NJS::MemberInfo NJS::Type::GetMember(const SourceLocation &where, unsigned) const
{
    Error(where, "type {} does not support 'GetMember'", m_String);
}

NJS::TypePtr NJS::Type::GetResultType(const SourceLocation &where) const
{
    Error(where, "type {} does not support 'GetResultType'", m_String);
}

NJS::TypePtr NJS::Type::GetParameterType(const SourceLocation &where, unsigned) const
{
    Error(where, "type {} does not support 'GetParameterType'", m_String);
}

unsigned NJS::Type::GetParameterCount(const SourceLocation &where) const
{
    Error(where, "type {} does not support 'GetParameterCount'", m_String);
}

bool NJS::Type::IsVarArg(const SourceLocation &where) const
{
    Error(where, "type {} does not support 'IsVarArg'", m_String);
}

NJS::Type::Type(TypeContext &type_context, std::string string)
    : m_TypeContext(type_context),
      m_String(std::move(string)),
      m_LLVM(nullptr),
      m_Size(~0u)
{
}
