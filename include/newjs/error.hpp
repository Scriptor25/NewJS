#pragma once

#include <format>
#include <memory>
#include <string_view>
#include <newjs/location.hpp>

namespace NJS
{
    class RTError final
    {
    public:
        RTError(SourceLocation where, std::string message);
        RTError(SourceLocation where, std::string message, RTError cause);
        RTError(std::string message, RTError cause);
        explicit RTError(std::string message);

        std::ostream &Print(std::ostream &stream) const;

    private:
        SourceLocation m_Where;
        std::string m_Message;
        std::shared_ptr<RTError> m_Cause;
    };

    [[noreturn]] void Error(SourceLocation where, RTError cause) noexcept(false);

    template<typename... Args>
    [[noreturn]] void Error(const std::string_view &format, Args &&... args) noexcept(false)
    {
        auto message = std::vformat(format, std::make_format_args(args...));
        throw RTError(std::move(message));
    }

    template<typename... Args>
    [[noreturn]] void Error(
        SourceLocation where,
        const std::string_view &format,
        Args &&... args) noexcept(false)
    {
        auto message = std::vformat(format, std::make_format_args(args...));
        throw RTError(std::move(where), std::move(message));
    }

    template<typename... Args>
    [[noreturn]] void Error(
        RTError cause,
        const std::string_view &format,
        Args &&... args) noexcept(false)
    {
        auto message = std::vformat(format, std::make_format_args(args...));
        throw RTError(std::move(message), std::move(cause));
    }
}
