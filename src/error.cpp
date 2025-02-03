#include <filesystem>
#include <iostream>
#include <NJS/Error.hpp>
#include <NJS/SourceLocation.hpp>

void NJS::Error(const std::string &message) noexcept
{
    std::cerr << message << std::endl;
    exit(1);
}

void NJS::Error(const SourceLocation &where, const std::string &message) noexcept
{
    const auto filename = where.Filename.empty()
                              ? "<empty>"
                              : std::filesystem::absolute(where.Filename).string();
    std::cerr << "at " << filename << ':' << where.Row << ':' << where.Col << ": " << message << std::endl;
    exit(1);
}
