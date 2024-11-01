#pragma once

#include <ostream>

namespace NJS
{
    struct SourceLocation
    {
        std::string Filename;
        size_t Row = 0;
        size_t Col = 0;
    };

    std::ostream& operator<<(std::ostream&, const SourceLocation&);
}
