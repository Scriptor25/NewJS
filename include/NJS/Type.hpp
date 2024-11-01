#pragma once

#include <format>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace NJS
{
    typedef std::shared_ptr<struct Type> TypePtr;

    std::ostream& operator<<(std::ostream&, const TypePtr&);

    struct Type
    {
        explicit Type(std::string);
        virtual ~Type() = default;

        virtual TypePtr Member(const std::string&);
        virtual TypePtr Result();
        virtual TypePtr Element();
        virtual TypePtr Element(size_t);
        virtual std::ostream& Print(std::ostream&);

        std::string String;
    };

    struct MultiType : Type
    {
        static std::string GenString(const std::vector<TypePtr>&);

        explicit MultiType(const std::vector<TypePtr>&);

        std::vector<TypePtr> Types;
    };

    enum TypeName
    {
        Type_Void,
        Type_Undefined,
        Type_Boolean,
        Type_Number,
        Type_String,
    };

    struct PrimitiveType : Type
    {
        static std::string GenString(TypeName);

        explicit PrimitiveType(TypeName);

        TypeName Name;
    };

    struct ArrayType : Type
    {
        static std::string GenString(const TypePtr&);

        explicit ArrayType(TypePtr);
        TypePtr Element() override;
        TypePtr Element(size_t) override;

        TypePtr ElementType;
    };

    struct TupleType : Type
    {
        static std::string GenString(const std::vector<TypePtr>&);

        TupleType(TypePtr, std::vector<TypePtr>);
        TypePtr Element() override;
        TypePtr Element(size_t) override;

        TypePtr ElementType;
        std::vector<TypePtr> ElementTypes;
    };

    struct ObjectType : Type
    {
        static std::string GenString(const std::map<std::string, TypePtr>&);

        explicit ObjectType(std::map<std::string, TypePtr>);
        TypePtr Member(const std::string&) override;

        std::map<std::string, TypePtr> ElementTypes;
    };

    struct FunctionType : Type
    {
        static std::string GenString(const std::vector<TypePtr>&, const TypePtr&);

        FunctionType(std::vector<TypePtr>, TypePtr);
        TypePtr Result() override;

        std::vector<TypePtr> ParamTypes;
        TypePtr ResultType;
    };
}

namespace std
{
    template <>
    struct formatter<NJS::TypePtr> : formatter<string>
    {
        template <typename FormatContext>
        auto format(const NJS::TypePtr& type, FormatContext& ctx) const
        {
            return formatter<string>::format(type->String, ctx);
        }
    };
}
