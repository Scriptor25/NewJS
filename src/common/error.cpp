#include <newjs/error.hpp>

NJS::RTError::RTError(SourceLocation where, std::string message)
    : m_Where(std::move(where)),
      m_Message(std::move(message))
{
}

NJS::RTError::RTError(SourceLocation where, std::string message, RTError &&cause)
    : m_Where(std::move(where)),
      m_Message(std::move(message)),
      m_Cause(std::make_unique<RTError>(std::move(cause)))
{
}

std::ostream &NJS::RTError::Print(std::ostream &stream) const
{
    auto &[
        filename_,
        row_,
        column_
    ] = m_Where;

    const auto not_redundant = !m_Cause || m_Cause->m_Where != m_Where;
    const auto has_where = !filename_.empty() && not_redundant;
    const auto has_message = !m_Message.empty();

    if (has_where)
        stream << "at " << filename_ << ':' << row_ << ':' << column_ << ": ";
    if (has_message)
        stream << m_Message;
    if (has_message || has_where)
        stream << std::endl;

    if (m_Cause)
        m_Cause->Print(stream);
    return stream;
}

void NJS::Error(const SourceLocation &where, RTError &&cause) noexcept(false)
{
    throw RTError(where, {}, std::move(cause));
}
