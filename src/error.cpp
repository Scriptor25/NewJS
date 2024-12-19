#include <filesystem>
#include <iostream>
#include <NJS/Error.hpp>
#include <NJS/SourceLocation.hpp>

void NJS::Error(const std::string& message)
{
    std::cerr << message << std::endl;
    throw std::runtime_error(message);
}

void NJS::Error(const SourceLocation& where, const std::string& message)
{
    const auto filename = std::filesystem::absolute(where.Filename).string();
    std::cerr << "at " << filename << ':' << where.Row << ':' << where.Col << ": " << message << std::endl;
    throw std::runtime_error(message);
}
