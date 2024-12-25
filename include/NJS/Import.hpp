#pragma once

#include <map>
#include <string>
#include <vector>
#include <NJS/NJS.hpp>

namespace NJS
{
    struct ImportMapping
    {
        std::ostream& Print(std::ostream&) const;
        void MapFunctions(Builder&, const SourceLocation&, const std::string&, const std::vector<StmtPtr>&) const;

        bool All;
        std::string Name;
        std::map<std::string, std::string> NameMap;
    };
}
