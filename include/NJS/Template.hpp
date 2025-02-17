#pragma once

#include <string>
#include <vector>
#include <NJS/AST.hpp>
#include <NJS/NJS.hpp>
#include <NJS/SourceLocation.hpp>

namespace NJS
{
    struct Template
    {
        std::string GetName(const std::vector<TypePtr> &arguments) const;

        FunctionStatement InflateFunction(Parser &parent, const std::vector<TypePtr> &arguments) const;
        TypePtr InflateType(Parser &parent, const std::vector<TypePtr> &arguments) const;

        SourceLocation Where;
        std::string Name;
        std::vector<std::string> Parameters;
        std::string Source;
    };
}
