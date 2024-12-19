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
        void MapFunctions(Parser&, const std::vector<StmtPtr>&) const;
        void MapFunctions(Builder&, const std::string&, const std::vector<StmtPtr>&) const;

        std::string Name;
        std::map<std::string, std::string> NameMap;
    };
}
