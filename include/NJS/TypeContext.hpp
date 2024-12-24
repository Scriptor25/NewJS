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

        NoTypePtr GetNoType();
        VoidTypePtr GetVoidType();
        IntTypePtr GetIntType(unsigned, bool);
        FPTypePtr GetFPType(unsigned);
        PtrTypePtr GetPointerType(const TypePtr&);
        RefTypePtr GetRefType(TypePtr);
        ArrayTypePtr GetArrayType(const TypePtr&, unsigned);
        StructTypePtr GetStructType(const std::map<std::string, TypePtr>&);
        TupleTypePtr GetTupleType(const std::vector<TypePtr>&);
        FunctionTypePtr GetFunctionType(const TypePtr&, const std::vector<TypePtr>&, bool);

        IntTypePtr GetBoolType();
        IntTypePtr GetCharType();
        PtrTypePtr GetStringType();

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
