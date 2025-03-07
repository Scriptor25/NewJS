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
         * @param values a list of functions
         */
        void MapValues(
            Builder &builder,
            const std::string &module_id,
            const std::vector<StatementPtr> &values) const;

        static void MapFunction(
            Builder &builder,
            const FunctionStatement &function,
            const std::string &module_id,
            ValuePtr &value,
            std::string &name,
            ReferenceInfo &info);
        static void MapVariable(
            Builder &builder,
            const VariableStatement &variable,
            const std::string &module_id,
            ValuePtr &value,
            std::string &name,
            ReferenceInfo &info);

        bool All{};
        std::string Name;
        std::map<std::string, std::string> NameMap;
    };
}
