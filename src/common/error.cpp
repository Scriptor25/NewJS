#include <newjs/error.hpp>

NJS::ErrorInfo::ErrorInfo(SourceLocation where, std::string message)
    : m_HasError(true),
      m_Where(std::move(where)),
      m_Message(std::move(message))
{
}

NJS::ErrorInfo::ErrorInfo(const ErrorInfo &cause, SourceLocation where, std::string message)
    : m_HasError(true),
      m_Cause(cause),
      m_Where(std::move(where)),
      m_Message(std::move(message))
{
}

NJS::ErrorInfo::operator bool() const
{
    return m_HasError;
}

std::ostream &NJS::ErrorInfo::Print(std::ostream &stream) const
{
    auto &[filename_, row_, column_] = m_Where;
    stream << "at " << filename_ << ':' << row_ << ':' << column_ << ": " << m_Message;
    if (m_Cause.has_value())
        m_Cause->Print(stream);
    return stream;
}
