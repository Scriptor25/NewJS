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
        RTError(SourceLocation where, std::string message, RTError &&cause);

        std::ostream &Print(std::ostream &stream) const;

    private:
        SourceLocation m_Where;
        std::string m_Message;
        std::unique_ptr<RTError> m_Cause;
    };

    [[noreturn]] void Error(const SourceLocation &where, RTError &&cause) noexcept(false);

    template<typename... Args>
    [[noreturn]] void Error(const std::string_view &format, Args &&... args) noexcept(false)
    {
        auto message = std::vformat(format, std::make_format_args(args...));
        throw RTError({}, message);
    }

    template<typename... Args>
    [[noreturn]] void Error(
        const SourceLocation &where,
        const std::string_view &format,
        Args &&... args) noexcept(false)
    {
        auto message = std::vformat(format, std::make_format_args(args...));
        throw RTError(where, message);
    }

    template<typename... Args>
    [[noreturn]] void Error(
        RTError &cause,
        const std::string_view &format,
        Args &&... args) noexcept(false)
    {
        auto message = std::vformat(format, std::make_format_args(args...));
        throw RTError({}, message, std::move(cause));
    }
}
