#pragma once

#include <string>

namespace NJS
{
    struct SourceLocation
    {
        SourceLocation() = default;
        explicit SourceLocation(const std::string &);

        std::string Filename;
        unsigned Row = 0;
        unsigned Col = 0;
    };
}
