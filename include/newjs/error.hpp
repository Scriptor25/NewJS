#pragma once

#include <format>
#include <iostream>
#include <memory>
#include <string_view>
#include <vector>
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
    [[noreturn]] void Error(const SourceLocation &where, const std::string_view &format, Args &&... args) noexcept
    {
        auto message = std::vformat(format, std::make_format_args(args...));
        std::cerr << "at " << where.Filename << ':' << where.Row << ':' << where.Col << ": " << message << std::endl;
        exit(1);
    }
}
