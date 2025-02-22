#pragma once

#include <format>
#include <string_view>
#include <NJS/NJS.hpp>

namespace NJS
{
    [[noreturn]] void Error(const std::string_view &message) noexcept;
    [[noreturn]] void Error(const SourceLocation &where, const std::string_view &message) noexcept;

    template<typename... Args>
    [[noreturn]] void Error(const SourceLocation &where, const std::string_view &format, Args &&... args) noexcept
    {
        const auto message = std::vformat(format, std::make_format_args(args...));
        Error(where, message);
    }

    template<typename... Args>
    [[noreturn]] void Error(const std::string_view &format, Args &&... args) noexcept
    {
        const auto message = std::vformat(format, std::make_format_args(args...));
        Error(message);
    }
}
