#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <NJS/NJS.hpp>

namespace NJS
{
    enum ParameterFlags
    {
        ParameterFlags_None = 0,
        ParameterFlags_Const = 1,
        ParameterFlags_Extern = 2,
    };

    struct Parameter
    {
        explicit Parameter(std::string name);
        virtual ~Parameter() = default;

        virtual bool RequireValue();
        virtual void CreateVars(
            Builder &builder,
            const SourceLocation &where,
            ValuePtr value,
            unsigned flags);

        virtual std::ostream &Print(std::ostream &stream);

        std::string Name;
        TypePtr Type;
    };

    struct DestructureObject final : Parameter
    {
        explicit DestructureObject(std::map<std::string, ParameterPtr> elements);

        bool RequireValue() override;
        void CreateVars(
            Builder &builder,
            const SourceLocation &where,
            ValuePtr value,
            unsigned flags) override;

        std::ostream &Print(std::ostream &stream) override;

        std::map<std::string, ParameterPtr> Elements;
    };

    struct DestructureArray final : Parameter
    {
        explicit DestructureArray(std::vector<ParameterPtr> elements);

        bool RequireValue() override;
        void CreateVars(
            Builder &builder,
            const SourceLocation &where,
            ValuePtr value,
            unsigned flags) override;

        std::ostream &Print(std::ostream &stream) override;

        std::vector<ParameterPtr> Elements;
    };
}
