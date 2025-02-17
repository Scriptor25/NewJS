#pragma once

#include <map>
#include <string>
#include <vector>
#include <NJS/NJS.hpp>
#include <NJS/Template.hpp>
#include <NJS/Type.hpp>

namespace NJS
{
    class TemplateContext
    {
    public:
        explicit TemplateContext(Builder &builder);

        void InsertType(
            const SourceLocation &where,
            const std::string &name,
            const std::vector<std::string> &parameters,
            const std::string &source);
        void InsertFunction(
            const SourceLocation &where,
            const std::string &name,
            const std::vector<std::string> &parameters,
            const std::string &source);

        [[nodiscard]] bool HasFunction(const std::string &name) const;
        [[nodiscard]] bool HasType(const std::string &name) const;

        std::string InflateFunction(
            Parser &parent,
            const std::string &template_name,
            const std::vector<TypePtr> &arguments);
        TypePtr InflateType(
            Parser &parent,
            const std::string &template_name,
            const std::vector<TypePtr> &arguments);

    private:
        Builder &m_Builder;

        std::map<std::string, Template> m_TypeTemplates;
        std::map<std::string, Template> m_FunctionTemplates;

        std::set<std::string> m_InflatedFunctions;
        std::map<std::string, TypePtr> m_InflatedTypes;
    };
}
