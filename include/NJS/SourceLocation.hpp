#pragma once

#include <string>

namespace NJS
{
    struct SourceLocation
    {
        std::string Filename;
        size_t Row = 0;
        size_t Col = 0;
    };
}
