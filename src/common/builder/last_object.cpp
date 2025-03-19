#include <newjs/builder.hpp>

void NJS::Builder::ClearLastObject()
{
    m_LastObject = nullptr;
}

void NJS::Builder::SetLastObject(const ValuePtr &object)
{
    m_LastObject = object;
}

NJS::ValuePtr NJS::Builder::GetLastObject() const
{
    return m_LastObject;
}
