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
        TypeContext();

        TypePtr& GetType(const std::string&);

        NoTypePtr GetNoType();

        PrimitiveTypePtr GetPrimitiveType(Primitive);
        PrimitiveTypePtr GetVoidType();
        PrimitiveTypePtr GetBooleanType();
        PrimitiveTypePtr GetNumberType();
        PrimitiveTypePtr GetStringType();
        PrimitiveTypePtr GetCharType();

        TupleTypePtr GetTupleType(const std::vector<TypePtr>&);
        ObjectTypePtr GetObjectType(const std::map<std::string, TypePtr>&);
        ArrayTypePtr GetArrayType(const TypePtr&, size_t);
        FunctionTypePtr GetFunctionType(const std::vector<TypePtr>&, const TypePtr&, bool);
        VectorTypePtr GetVectorType(const TypePtr&);

    private:
        template <typename T, typename... Args>
        std::shared_ptr<T> GetType(Args&&... args)
        {
            auto& ref = GetType(T::GenString(args...));
            if (ref) return std::dynamic_pointer_cast<T>(ref);
            auto type = std::make_shared<T>(*this, args...);
            ref = type;
            return type;
        }

        std::map<std::string, TypePtr> m_TypeMap;
    };
}
