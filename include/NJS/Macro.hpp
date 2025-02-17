#pragma once

#include <string>
#include <vector>
#include <NJS/AST.hpp>

namespace NJS
{
    struct Macro
    {
        ExpressionPtr Inflate(Parser &parent) const;

        std::vector<std::string> Parameters;
        std::string Source;
    };
}
