#pragma once

#include <string>
#include <vector>
#include <newjs/ast.hpp>

namespace NJS
{
    struct Macro
    {
        ExpressionPtr Inflate(Parser &parent) const;

        SourceLocation Where;
        std::vector<std::string> Parameters;
        std::string Source;
    };
}
