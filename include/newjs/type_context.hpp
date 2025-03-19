#pragma once

#include <map>
#include <memory>
#include <vector>
#include <newjs/newjs.hpp>
#include <newjs/type.hpp>

namespace NJS
{
    class TypeContext
    {
    public:
        [[nodiscard]] bool HasNamedType(const std::string &string) const;
        [[nodiscard]] TypePtr GetNamedType(const std::string &string) const;
        TypePtr &GetNamedTypeReference(const std::string &string);

        [[nodiscard]] TypePtr GetType(unsigned hash) const;
        TypePtr &GetTypeReference(unsigned hash);

        VoidTypePtr GetVoidType();

        IntegerTypePtr GetIntegerType(unsigned bits, bool is_signed);

        FloatingPointTypePtr GetFloatingPointType(unsigned bits);

        PointerTypePtr GetPointerType(const TypePtr &element_type, bool is_const);

        ArrayTypePtr GetArrayType(const TypePtr &element_type, unsigned count);

        StructTypePtr GetStructType(const std::string &name);
        StructTypePtr GetStructType(
            const std::vector<std::pair<std::string, TypePtr>> &element_types,
            const std::string &name);
        StructTypePtr GetStructType(const std::vector<StructElement> &elements, std::string name);

        TupleTypePtr GetTupleType(const std::vector<TypePtr> &element_types);

        FunctionTypePtr GetFunctionType(
            const ReferenceInfo &result,
            const std::vector<ReferenceInfo> &parameters,
            bool is_var_arg);
        FunctionTypePtr GetFunctionType(
            const ReferenceInfo &result,
            const std::vector<ParameterPtr> &parameters,
            bool is_var_arg);

        TypePtr GetLambdaType(const std::vector<StructElement> &elements, const FunctionTypePtr &function_type);

        IntegerTypePtr GetBooleanType();
        IntegerTypePtr GetCharType();
        PointerTypePtr GetStringType();

    private:
        template<typename T, typename... Args>
        std::shared_ptr<T> GetType(Args &&... args)
        {
            auto hash = T::GenHash(args...);
            auto &dest = GetTypeReference(hash);
            if (!dest)
            {
                auto string = T::GenString(args...);
                auto type_pointer = new T(*this, hash, string, std::forward<Args>(args)...);
                dest = std::shared_ptr<T>(type_pointer);
            }
            return Type::As<T>(dest);
        }

        std::map<unsigned, TypePtr> m_Types;
        std::map<std::string, TypePtr> m_NamedTypes;
    };
}
