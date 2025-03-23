#include <newjs/builder.hpp>

void NJS::Builder::PushLastObjectContext()
{
    m_LastObject.emplace_back();
}

void NJS::Builder::PopLastObjectContext()
{
    m_LastObject.pop_back();
}

void NJS::Builder::SetLastObject(const ValuePtr &object)
{
    if (m_LastObject.empty() || m_LastObject.back())
        return;
    m_LastObject.back() = object;
}

NJS::ValuePtr NJS::Builder::GetLastObject() const
{
    if (m_LastObject.empty())
        return nullptr;
    return m_LastObject.back();
}
