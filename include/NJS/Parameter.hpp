#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <NJS/NJS.hpp>
#include <NJS/SourceLocation.hpp>

namespace NJS
{
    struct Parameter
    {
        Parameter(
            SourceLocation where,
            std::string name,
            TypePtr type);
        virtual ~Parameter() = default;

        virtual bool RequireValue();
        virtual void CreateVars(
            Builder &builder,
            ValuePtr value,
            bool is_extern,
            bool is_const,
            bool is_reference);

        virtual std::ostream &Print(std::ostream &stream);

        SourceLocation Where;
        std::string Name;
        TypePtr Type;
    };

    struct DestructureStruct final : Parameter
    {
        DestructureStruct(
            SourceLocation where,
            std::map<std::string, ParameterPtr> elements,
            TypePtr type);

        bool RequireValue() override;
        void CreateVars(
            Builder &builder,
            ValuePtr value,
            bool is_extern,
            bool is_const,
            bool is_reference) override;

        std::ostream &Print(std::ostream &stream) override;

        std::map<std::string, ParameterPtr> Elements;
    };

    struct DestructureTuple final : Parameter
    {
        DestructureTuple(
            SourceLocation where,
            std::vector<ParameterPtr> elements,
            TypePtr type);

        bool RequireValue() override;
        void CreateVars(
            Builder &builder,
            ValuePtr value,
            bool is_extern,
            bool is_const,
            bool is_reference) override;

        std::ostream &Print(std::ostream &stream) override;

        std::vector<ParameterPtr> Elements;
    };
}
