#pragma once

#include <map>
#include <string>
#include <vector>
#include <NJS/NJS.hpp>

namespace NJS
{
    struct ImportMapping
    {
        std::ostream &Print(std::ostream &stream) const;
        void MapFunctions(
            Builder &builder,
            const SourceLocation &where,
            const std::string &module_id,
            const std::vector<FunctionStatementPtr> &functions) const;

        bool All{};
        std::string Name;
        std::map<std::string, std::string> NameMap;
    };
}
