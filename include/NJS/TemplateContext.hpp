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
        explicit TemplateContext(Builder &);

        void InsertType(
            const std::string &,
            const std::vector<std::string> &,
            const SourceLocation &,
            const std::string &);
        void InsertFunction(
            const std::string &,
            const std::vector<std::string> &,
            const SourceLocation &,
            const std::string &);

        [[nodiscard]] bool HasFunction(const std::string &) const;
        [[nodiscard]] bool HasType(const std::string &) const;

        std::string InflateFunctionTemplate(Parser &, const std::string &, const std::vector<TypePtr> &);
        TypePtr InflateType(Parser &, const std::string &, const std::vector<TypePtr> &);

    private:
        Builder &m_Builder;

        std::map<std::string, Template> m_TypeTemplates;
        std::map<std::string, Template> m_FunctionTemplates;

        std::map<std::string, StmtPtr> m_InflatedFunctions;
        std::map<std::string, TypePtr> m_InflatedTypes;
    };
}
