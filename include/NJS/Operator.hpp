#pragma once

#include <memory>

namespace NJS
{
    class Context;
    typedef std::shared_ptr<struct Type> TypePtr;

    struct Operator
    {
        static Operator& Get(const std::string&);

        TypePtr Result(Context&, const TypePtr&, const TypePtr&) const;

        std::string Id;
        int Precedence;
    };
}
