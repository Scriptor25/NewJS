#pragma once

#include <string>
#include <vector>
#include <newjs/ast.hpp>
#include <newjs/location.hpp>
#include <newjs/newjs.hpp>

namespace NJS
{
    struct Template
    {
        [[nodiscard]] std::string GetName(const std::vector<TypePtr> &arguments) const;

        FunctionStatement InflateFunction(const Parser &parent, const std::vector<TypePtr> &arguments) const;
        TypePtr InflateType(const Parser &parent, const std::vector<TypePtr> &arguments) const;

        SourceLocation Where;
        std::string Name;
        std::vector<std::string> Parameters;
        std::string Source;
    };
}
