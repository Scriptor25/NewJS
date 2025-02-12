#pragma once

#include <map>
#include <string>
#include <vector>
#include <NJS/NJS.hpp>
#include <NJS/SourceLocation.hpp>
#include <NJS/Type.hpp>

namespace NJS
{
    struct Template
    {
        std::string Name;
        std::vector<std::string> Args;
        SourceLocation Where;
        std::string Source;
    };

    class TemplateContext
    {
    public:
        explicit TemplateContext(Builder &builder);

        void InsertType(
            const std::string_view &name,
            const std::vector<std::string> &args,
            const SourceLocation &where,
            const std::string_view &source);
        void InsertFunction(
            const std::string_view &name,
            const std::vector<std::string> &args,
            const SourceLocation &where,
            const std::string_view &source);

        [[nodiscard]] bool HasFunction(const std::string_view &name) const;
        [[nodiscard]] bool HasType(const std::string_view &name) const;

        std::string InflateFunctionTemplate(
            Parser &parent,
            const std::string &templ_name,
            const std::vector<TypePtr> &args);
        TypePtr InflateType(
            Parser &parent,
            const std::string &templ_name,
            const std::vector<TypePtr> &args);

    private:
        Builder &m_Builder;

        std::map<std::string, Template> m_TypeTemplates;
        std::map<std::string, Template> m_FunctionTemplates;

        std::map<std::string, StatementPtr> m_InflatedFunctions;
        std::map<std::string, TypePtr> m_InflatedTypes;
    };
}
