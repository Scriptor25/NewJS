#pragma once

#include <string>
#include <vector>
#include <NJS/NJS.hpp>

namespace NJS
{
    struct ImportMapping
    {
        std::ostream& Print(std::ostream&) const;
        void MapFunctions(Builder&, const std::string&, const std::vector<FunctionStmtPtr>&) const;

        std::string Name;
        TypePtr Type;
        std::map<std::string, std::string> SubMappings;
    };
}
