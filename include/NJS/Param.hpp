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
        explicit Param(std::string_view name);
        virtual ~Param() = default;

        virtual bool RequireValue();
        virtual void CreateVars(Builder &builder, const SourceLocation &where, bool is_const, const ValuePtr &value);

        virtual std::ostream &Print(std::ostream &stream);

        std::string Name;
        TypePtr Type;
    };

    struct DestructureObject final : Param
    {
        explicit DestructureObject(std::map<std::string, ParamPtr> elements);

        bool RequireValue() override;
        void CreateVars(Builder &builder, const SourceLocation &where, bool is_const, const ValuePtr &value) override;

        std::ostream &Print(std::ostream &stream) override;

        std::map<std::string, ParamPtr> Elements;
    };

    struct DestructureArray final : Param
    {
        explicit DestructureArray(std::vector<ParamPtr> elements);

        bool RequireValue() override;
        void CreateVars(Builder &builder, const SourceLocation &where, bool is_const, const ValuePtr &value) override;

        std::ostream &Print(std::ostream &stream) override;

        std::vector<ParamPtr> Elements;
    };
}
