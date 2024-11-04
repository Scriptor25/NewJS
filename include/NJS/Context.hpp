#pragma once

#include <map>
#include <memory>
#include <vector>
#include <NJS/Type.hpp>

namespace NJS
{
    class Context
    {
    public:
        Context();

        TypePtr& GetType(const std::string&);
        TypePtr GetPrimitiveType(TypeName);

        TypePtr GetVoidType();
        TypePtr GetBooleanType();
        TypePtr GetNumberType();
        TypePtr GetStringType();
        TypePtr GetTupleType(const std::vector<TypePtr>&);
        TypePtr GetObjectType(const std::map<std::string, TypePtr>&);
        TypePtr GetArrayType(const TypePtr&);
        TypePtr GetFunctionType(const std::vector<TypePtr>&, const TypePtr&, bool);

    private:
        std::map<std::string, TypePtr> m_TypeMap;
    };
}
