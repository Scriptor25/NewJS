#pragma once

#include <map>
#include <string>
#include <vector>
#include <newjs/newjs.hpp>

namespace NJS
{
    struct ImportMapping
    {
        std::ostream &Print(std::ostream &stream) const;

        /**
         * This helper function maps a list of functions into the import mapping for a module.
         *
         * @param builder current builder instance
         * @param module_id id of the module
         * @param functions a list of functions
         */
        void MapFunctions(
            Builder &builder,
            const std::string &module_id,
            const std::vector<FunctionStatementPtr> &functions) const;

        bool All{};
        std::string Name;
        std::map<std::string, std::string> NameMap;
    };
}
