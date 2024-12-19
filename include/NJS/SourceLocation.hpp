#pragma once

#include <string>

namespace NJS
{
    struct SourceLocation
    {
        std::string Filename;
        unsigned Row = 0;
        unsigned Col = 0;
    };
}
