#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <NJS/NJS.hpp>

namespace NJS
{
    struct Parameter
    {
        explicit Parameter(std::string_view name);
        virtual ~Parameter() = default;

        virtual bool RequireValue();
        virtual void CreateVars(Builder &builder, const SourceLocation &where, bool is_const, const ValuePtr &value);

        virtual std::ostream &Print(std::ostream &stream);

        std::string Name;
        TypePtr Type;
    };

    struct DestructureObject final : Parameter
    {
        explicit DestructureObject(std::map<std::string, ParameterPtr> elements);

        bool RequireValue() override;
        void CreateVars(Builder &builder, const SourceLocation &where, bool is_const, const ValuePtr &value) override;

        std::ostream &Print(std::ostream &stream) override;

        std::map<std::string, ParameterPtr> Elements;
    };

    struct DestructureArray final : Parameter
    {
        explicit DestructureArray(std::vector<ParameterPtr> elements);

        bool RequireValue() override;
        void CreateVars(Builder &builder, const SourceLocation &where, bool is_const, const ValuePtr &value) override;

        std::ostream &Print(std::ostream &stream) override;

        std::vector<ParameterPtr> Elements;
    };
}
