#pragma once

#include <format>
#include <functional>
#include <iostream>
#include <ostream>
#include <NJS/SourceLocation.hpp>

namespace NJS
{
    void Indent();
    void Exdent();
    std::ostream& Spacing(std::ostream&);

    template <typename... Args>
    [[noreturn]] void Error(const SourceLocation& where, const std::string& format, Args&&... args)
    {
        const auto message = std::vformat(format, std::make_format_args(args...));
        std::cerr << "at " << where << ": " << message << std::endl;
        throw std::runtime_error("NJS::Error");
    }

    template <typename... Args>
    [[noreturn]] void Error(const std::string& format, Args&&... args)
    {
        const auto message = std::vformat(format, std::make_format_args(args...));
        std::cerr << message << std::endl;
        throw std::runtime_error("NJS::Error");
    }

    [[noreturn]] inline void Error(const SourceLocation& where)
    {
        std::cerr << "at " << where << std::endl;
        throw std::runtime_error("NJS::Error");
    }

    template <typename R>
    R Catch(const std::function<R()>& t, const std::function<void()>& c)
    {
        try { return t(); }
        catch (std::exception&)
        {
            c();
            throw;
        }
    }
}
