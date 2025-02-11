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
        TypePtr &GetType(const std::string &);

        NoTypePtr GetNoType(const std::string & = {});
        VoidTypePtr GetVoidType();
        IntTypePtr GetIntType(unsigned, bool);
        FPTypePtr GetFPType(unsigned);
        PtrTypePtr GetPointerType(const TypePtr &);
        RefTypePtr GetRefType(const TypePtr &);
        ArrayTypePtr GetArrayType(const TypePtr &, unsigned);
        StructTypePtr GetStructType(const std::map<std::string, TypePtr> &);
        TupleTypePtr GetTupleType(const std::vector<TypePtr> &);
        FunctionTypePtr GetFunctionType(const TypePtr &, const std::vector<TypePtr> &, bool);

        IntTypePtr GetBoolType();
        IntTypePtr GetCharType();
        PtrTypePtr GetStringType();

        void PushTemplate(const std::vector<std::string> &, const std::vector<TypePtr> &);
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
        std::vector<std::map<std::string, TypePtr> > m_TemplateStack;
    };
}
