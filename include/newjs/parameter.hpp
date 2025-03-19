#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <newjs/info.hpp>
#include <newjs/location.hpp>
#include <newjs/newjs.hpp>

namespace NJS
{
    struct Parameter
    {
        Parameter(
            SourceLocation where,
            std::string name,
            ReferenceInfo info);
        virtual ~Parameter() = default;

        virtual bool RequireValue();
        virtual void CreateVars(
            Builder &builder,
            ValuePtr value,
            bool is_export,
            bool is_extern,
            bool is_const,
            bool is_reference);

        virtual std::ostream &Print(std::ostream &stream, bool with_info);

        SourceLocation Where;
        std::string Name;
        ReferenceInfo Info;
    };

    struct DestructureStruct final : Parameter
    {
        DestructureStruct(
            SourceLocation where,
            std::map<std::string, ParameterPtr> elements,
            bool all,
            ReferenceInfo info);

        bool RequireValue() override;
        void CreateVars(
            Builder &builder,
            ValuePtr value,
            bool is_export,
            bool is_extern,
            bool is_const,
            bool is_reference) override;

        std::ostream &Print(std::ostream &stream, bool with_info) override;

        std::map<std::string, ParameterPtr> Elements;
        bool All;
    };

    struct DestructureTuple final : Parameter
    {
        DestructureTuple(
            SourceLocation where,
            std::vector<ParameterPtr> elements,
            ReferenceInfo info);

        bool RequireValue() override;
        void CreateVars(
            Builder &builder,
            ValuePtr value,
            bool is_export,
            bool is_extern,
            bool is_const,
            bool is_reference) override;

        std::ostream &Print(std::ostream &stream, bool with_info) override;

        std::vector<ParameterPtr> Elements;
    };
}
