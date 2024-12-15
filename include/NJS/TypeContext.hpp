#pragma once

#include <map>
#include <memory>
#include <vector>
#include <NJS/NJS.hpp>
#include <NJS/Type.hpp>

namespace NJS
{
    class TypeContext
    {
    public:
        TypePtr& GetType(const std::string&);

        TypePtr GetVoidType();
        TypePtr GetIntType(unsigned, bool);
        TypePtr GetFPType(unsigned);
        TypePtr GetPointerType(TypePtr);
        TypePtr GetArrayType(TypePtr, unsigned);
        TypePtr GetStructType(std::map<std::string, TypePtr>);
        TypePtr GetTupleType(std::vector<TypePtr>);
        TypePtr GetFunctionType(TypePtr, std::vector<TypePtr>, bool);

    private:
        template <typename T, typename... Args>
        std::shared_ptr<T> GetType(Args&&... args)
        {
            auto string = T::GenString(args...);
            auto& ref = GetType(string);
            if (ref) return std::dynamic_pointer_cast<T>(ref);
            auto type = std::shared_ptr<T>(new T(*this, string, args...));
            ref = type;
            return type;
        }

        std::map<std::string, TypePtr> m_TypeMap;
    };
}
