#pragma once

#include <string>

namespace NJS
{
    struct SourceLocation
    {
        SourceLocation() = default;
        explicit SourceLocation(std::string filename);
        SourceLocation(std::string filename, unsigned row, unsigned column);

        explicit operator bool() const;

        bool operator==(const SourceLocation &other) const = default;

        std::string Filename;
        unsigned Row = 0;
        unsigned Column = 0;
    };
}
