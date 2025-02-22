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
        const TypePtr &GetType(const SourceLocation &where, const std::string &string) const;
        TypePtr &DefType(const std::string &string);

        IncompleteTypePtr GetIncompleteType(const std::string &name = {});
        VoidTypePtr GetVoidType();
        IntegerTypePtr GetIntegerType(unsigned bits, bool is_signed);
        FloatingPointTypePtr GetFloatingPointType(unsigned bits);
        PointerTypePtr GetPointerType(const TypePtr &element_type);
        ArrayTypePtr GetArrayType(const TypePtr &element_type, unsigned count);
        StructTypePtr GetStructType(const std::vector<std::pair<std::string, TypePtr>> &element_types);
        TupleTypePtr GetTupleType(const std::vector<TypePtr> &element_types);
        FunctionTypePtr GetFunctionType(
            const ReferenceInfo &result,
            const std::vector<ReferenceInfo> &parameters,
            bool is_var_arg);

        IntegerTypePtr GetBooleanType();
        IntegerTypePtr GetCharType();
        PointerTypePtr GetStringType();

        void PushTemplate(const std::vector<std::string> &names, const std::vector<TypePtr> &types);
        void PopTemplate();

    private:
        template<typename T, typename... Args>
        std::shared_ptr<T> GetType(Args &&... args)
        {
            auto string = T::GenString(args...);
            auto &ref = DefType(string);
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
