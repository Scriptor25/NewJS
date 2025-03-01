#pragma once

#include <format>
#include <vector>
#include <newjs/location.hpp>

namespace NJS
{
    struct ErrorFrame final
    {
        std::ostream &PrintWhere(std::ostream &stream) const;
        std::ostream &PrintMessage(std::ostream &stream) const;

        SourceLocation Where;
        std::string Message;
    };

    class RTError final : public std::exception
    {
    public:
        RTError() = default;
        RTError(const RTError &cause, const ErrorFrame &frame);

        std::ostream &Print(std::ostream &stream) const;

        [[nodiscard]] char const *what() const noexcept override;

    private:
        std::vector<ErrorFrame> m_Trace;
    };

    RTError operator+(const RTError &lhs, const ErrorFrame &rhs);


    [[noreturn]] void Error(const RTError &cause, SourceLocation where, std::string message);

    template<typename... Args>
    [[noreturn]] void Error(const std::string_view &format, Args &&... args)
    {
        auto message = std::vformat(format, std::make_format_args(args...));
        Error({}, {}, std::move(message));
    }

    template<typename... Args>
    [[noreturn]] void Error(
        SourceLocation where,
        const std::string_view &format,
        Args &&... args)
    {
        auto message = std::vformat(format, std::make_format_args(args...));
        Error({}, std::move(where), std::move(message));
    }

    template<typename... Args>
    [[noreturn]] void Error(
        RTError cause,
        SourceLocation where,
        const std::string_view &format,
        Args &&... args)
    {
        auto message = std::vformat(format, std::make_format_args(args...));
        Error(std::move(cause), std::move(where), std::move(message));
    }
}
