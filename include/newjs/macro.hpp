#pragma once

#include <string>
#include <vector>
#include <newjs/ast.hpp>

namespace NJS
{
    enum MacroParameterType
    {
        MacroParameterType_Statement,
        MacroParameterType_Expression,
        MacroParameterType_Type,
    };

    struct Macro
    {
        std::pair<SourceLocation, std::string> PrepareSource(Parser &parent) const;

        ExpressionPtr Inflate(Parser &parent) const;
        TypePtr InflateType(Parser &parent) const;

        SourceLocation Where;
        std::vector<std::pair<std::string, MacroParameterType>> Parameters;
        std::string Source;
    };
}
