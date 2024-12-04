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

        std::shared_ptr<PrimitiveType> GetPrimitiveType(TypeName);
        std::shared_ptr<PrimitiveType> GetVoidType();
        std::shared_ptr<PrimitiveType> GetBooleanType();
        std::shared_ptr<PrimitiveType> GetNumberType();
        std::shared_ptr<PrimitiveType> GetStringType();

        std::shared_ptr<TupleType> GetTupleType(const std::vector<TypePtr>&);
        std::shared_ptr<ObjectType> GetObjectType(const std::map<std::string, TypePtr>&);
        std::shared_ptr<ArrayType> GetArrayType(const TypePtr&, size_t);
        std::shared_ptr<FunctionType> GetFunctionType(const std::vector<TypePtr>&, const TypePtr&, bool);

    private:
        template <typename T, typename... Args>
        std::shared_ptr<T> GetType(Args&&... args)
        {
            auto& ref = GetType(T::GenString(args...));
            if (ref) return std::dynamic_pointer_cast<T>(ref);
            auto type = std::make_shared<T>(args...);
            ref = type;
            return type;
        }

        std::map<std::string, TypePtr> m_TypeMap;
    };
}
