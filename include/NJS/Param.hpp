#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <NJS/NJS.hpp>

namespace NJS
{
    struct Param
    {
        explicit Param(std::string);
        virtual ~Param() = default;

        virtual bool RequireValue();
        virtual void CreateVars(Builder &, const SourceLocation &, bool, const ValuePtr &);

        virtual std::ostream &Print(std::ostream &);

        std::string Name;
        TypePtr Type;
    };

    struct DestructureObject final : Param
    {
        explicit DestructureObject(std::map<std::string, ParamPtr>);

        bool RequireValue() override;
        void CreateVars(Builder &, const SourceLocation &, bool, const ValuePtr &) override;

        std::ostream &Print(std::ostream &) override;

        std::map<std::string, ParamPtr> Elements;
    };

    struct DestructureArray final : Param
    {
        explicit DestructureArray(std::vector<ParamPtr>);

        bool RequireValue() override;
        void CreateVars(Builder &, const SourceLocation &, bool, const ValuePtr &) override;

        std::ostream &Print(std::ostream &) override;

        std::vector<ParamPtr> Elements;
    };
}
