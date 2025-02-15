#pragma once

#include <map>
#include <memory>
#include <vector>
#include <NJS/NJS.hpp>

namespace NJS
{
    class TypeContext
    {
    public:
        TypePtr &GetType(const std::string &string);

        NoTypePtr GetNoType(const std::string &name = {});
        VoidTypePtr GetVoidType();
        IntegerTypePtr GetIntegerType(unsigned bits, bool is_signed);
        FloatingPointTypePtr GetFloatingPointType(unsigned bits);
        PointerTypePtr GetPointerType(const TypePtr &element_type);
        ReferenceTypePtr GetReferenceType(const TypePtr &element_type);
        ArrayTypePtr GetArrayType(const TypePtr &element_type, unsigned count);
        StructTypePtr GetStructType(const std::map<std::string, TypePtr> &element_type_map);
        TupleTypePtr GetTupleType(const std::vector<TypePtr> &element_types);
        FunctionTypePtr GetFunctionType(
            const TypePtr &result_type,
            const std::vector<TypePtr> &argument_types,
            bool var_arg);

        IntegerTypePtr GetBoolType();
        IntegerTypePtr GetCharType();
        PointerTypePtr GetStringType();

        void PushTemplate(const std::vector<std::string> &names, const std::vector<TypePtr> &types);
        void PopTemplate();

    private:
        template<typename T, typename... Args>
        std::shared_ptr<T> GetType(Args &&... args)
        {
            auto string = T::GenString(args...);
            auto &ref = GetType(string);
            if (!ref)
            {
                const auto ptr = new T(*this, string, std::forward<Args>(args)...);
                ref = std::shared_ptr<T>(ptr);
            }
            return std::dynamic_pointer_cast<T>(ref);
        }

        std::map<std::string, TypePtr> m_Types;
        std::vector<std::map<std::string, TypePtr>> m_TemplateStack;
    };
}
