#pragma once

#include <format>
#include <string>
#include <NJS/NJS.hpp>

namespace NJS
{
    [[noreturn]] void Error(const std::string&) noexcept;
    [[noreturn]] void Error(const SourceLocation&, const std::string&) noexcept;

    template <typename... Args>
    [[noreturn]] void Error(const SourceLocation& where, const std::string& format, Args&&... args) noexcept
    {
        const auto message = std::vformat(format, std::make_format_args(args...));
        Error(where, message);
    }

    template <typename... Args>
    [[noreturn]] void Error(const std::string& format, Args&&... args) noexcept
    {
        const auto message = std::vformat(format, std::make_format_args(args...));
        Error(message);
    }
}
