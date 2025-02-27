#pragma once

#include <format>
#include <iostream>
#include <optional>
#include <string_view>
#include <newjs/location.hpp>

namespace NJS
{
    template<typename... Args>
    [[noreturn]] void Error(const std::string_view &format, Args &&... args) noexcept
    {
        auto message = std::vformat(format, std::make_format_args(args...));
        std::cerr << message << std::endl;
        exit(1);
    }

    template<typename... Args>
    [[noreturn]] void Error(const SourceLocation where, const std::string_view &format, Args &&... args) noexcept
    {
        auto message = std::vformat(format, std::make_format_args(args...));
        std::cerr << "at " << where.Filename << ':' << where.Row << ':' << where.Col << ": " << message << std::endl;
        exit(1);
    }

    class ErrorInfo
    {
    public:
        ErrorInfo() = default;
        ErrorInfo(SourceLocation where, std::string message);
        ErrorInfo(const ErrorInfo &cause, SourceLocation where, std::string message);

        operator bool() const;

        std::ostream &Print(std::ostream &stream) const;

    private:
        bool m_HasError = false;
        std::optional<ErrorInfo> m_Cause;
        SourceLocation m_Where;
        std::string m_Message;
    };
}
