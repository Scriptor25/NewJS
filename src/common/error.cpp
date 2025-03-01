#include <iostream>
#include <ranges>
#include <newjs/error.hpp>

std::ostream &NJS::ErrorFrame::PrintWhere(std::ostream &stream) const
{
    auto &[
        filename_,
        row_,
        column_
    ] = Where;

    if (filename_.empty())
        return stream;

    return stream << "at " << filename_ << ':' << row_ << ':' << column_ << ": ";
}

std::ostream &NJS::ErrorFrame::PrintMessage(std::ostream &stream) const
{
    return stream << Message;
}

NJS::RTError::RTError(const RTError &cause, const ErrorFrame &frame)
{
    m_Trace.emplace_back(frame);
    for (const auto &trace_frame: cause.m_Trace)
    {
        auto &[
            where_,
            message_
        ] = m_Trace.back();
        if (where_ != trace_frame.Where && trace_frame.Where)
        {
            m_Trace.emplace_back(trace_frame);
        }
        else if (message_.empty() && !trace_frame.Message.empty())
        {
            message_ = trace_frame.Message;
        }
    }
}

std::ostream &NJS::RTError::Print(std::ostream &stream) const
{
    for (auto &frame: m_Trace)
    {
        frame.PrintWhere(stream);
        frame.PrintMessage(stream);
        stream << std::endl;
    }
    return stream;
}

char const *NJS::RTError::what() const noexcept
{
    return "RTError";
}

NJS::RTError NJS::operator+(const RTError &lhs, const ErrorFrame &rhs)
{
    return {lhs, rhs};
}

void NJS::Error(const RTError &cause, SourceLocation where, std::string message)
{
    throw cause + ErrorFrame(std::move(where), std::move(message));
}
