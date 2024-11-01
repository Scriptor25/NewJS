#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace NJS
{
    class Context;

    typedef std::shared_ptr<struct Type> TypePtr;
    typedef std::shared_ptr<struct Param> ParamPtr;

    std::ostream& operator<<(std::ostream&, const ParamPtr&);

    struct Param
    {
        explicit Param(std::string);
        virtual ~Param() = default;

        virtual bool RequireValue();
        virtual void CreateVars(Context&, const TypePtr&);

        virtual std::ostream& Print(std::ostream&);

        std::string Name;
        TypePtr Type;
    };

    struct DestructureObject : Param
    {
        explicit DestructureObject(std::map<std::string, ParamPtr>);

        bool RequireValue() override;
        void CreateVars(Context&, const TypePtr&) override;

        std::ostream& Print(std::ostream&) override;

        std::map<std::string, ParamPtr> Elements;
    };

    struct DestructureArray : Param
    {
        explicit DestructureArray(std::vector<ParamPtr>);

        bool RequireValue() override;
        void CreateVars(Context&, const TypePtr&) override;

        std::ostream& Print(std::ostream&) override;

        std::vector<ParamPtr> Elements;
    };
}
