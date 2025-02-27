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
            TypePtr type,
            ReferenceInfo info);
        virtual ~Parameter() = default;

        virtual bool RequireValue();
        virtual void CreateVars(
            Builder &builder,
            ValuePtr value,
            bool is_extern,
            bool is_const,
            bool is_reference,
            ErrorInfo &error);

        virtual std::ostream &Print(std::ostream &stream);

        SourceLocation Where;
        std::string Name;
        TypePtr Type;
        ReferenceInfo Info;
    };

    struct DestructureStruct final : Parameter
    {
        DestructureStruct(
            SourceLocation where,
            std::map<std::string, ParameterPtr> elements,
            TypePtr type,
            ReferenceInfo info);

        bool RequireValue() override;
        void CreateVars(
            Builder &builder,
            ValuePtr value,
            bool is_extern,
            bool is_const,
            bool is_reference,
            ErrorInfo &error) override;

        std::ostream &Print(std::ostream &stream) override;

        std::map<std::string, ParameterPtr> Elements;
    };

    struct DestructureTuple final : Parameter
    {
        DestructureTuple(
            SourceLocation where,
            std::vector<ParameterPtr> elements,
            TypePtr type,
            ReferenceInfo info);

        bool RequireValue() override;
        void CreateVars(
            Builder &builder,
            ValuePtr value,
            bool is_extern,
            bool is_const,
            bool is_reference,
            ErrorInfo &error) override;

        std::ostream &Print(std::ostream &stream) override;

        std::vector<ParameterPtr> Elements;
    };
}
