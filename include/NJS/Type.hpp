#pragma once

#include <format>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <NJS/NJS.hpp>

namespace NJS
{
    enum Primitive
    {
        Primitive_Void,
        Primitive_Boolean,
        Primitive_Number,
        Primitive_String,
        Primitive_Char,
    };

    struct Type
    {
        explicit Type(std::string);
        virtual ~Type() = default;

        virtual bool IsPrimitive() const;
        virtual bool IsPrimitive(Primitive) const;
        virtual bool IsArray() const;
        virtual bool IsTuple() const;
        virtual bool IsObject() const;
        virtual bool IsFunction() const;
        virtual bool IsVector() const;

        virtual TypePtr Member(const std::string&);
        virtual size_t MemberIndex(const std::string&);
        virtual TypePtr Result();
        virtual TypePtr Element();
        virtual TypePtr Element(size_t);
        virtual size_t NumElements() const;

        virtual void TypeInfo(Builder&, std::vector<llvm::Value*>&) const = 0;
        virtual size_t Bytes() const = 0;
        virtual llvm::Type* GenLLVM(Builder&) const = 0;

        std::ostream& Print(std::ostream&) const;

        std::string String;
    };

    struct PrimitiveType : Type
    {
        static std::string GenString(Primitive);

        explicit PrimitiveType(Primitive);

        bool IsPrimitive() const override;
        bool IsPrimitive(Primitive) const override;

        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;
        size_t Bytes() const override;
        llvm::Type* GenLLVM(Builder&) const override;

        Primitive Name;
    };

    struct ArrayType : Type
    {
        static std::string GenString(const TypePtr&, size_t);

        ArrayType(TypePtr, size_t);

        bool IsArray() const override;

        TypePtr Element() override;
        TypePtr Element(size_t) override;
        size_t NumElements() const override;

        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;
        size_t Bytes() const override;
        llvm::Type* GenLLVM(Builder&) const override;

        TypePtr ElementType;
        size_t ElementCount;
    };

    struct TupleType : Type
    {
        static std::string GenString(const std::vector<TypePtr>&);

        explicit TupleType(std::vector<TypePtr>);

        bool IsTuple() const override;

        TypePtr Element(size_t) override;
        size_t NumElements() const override;

        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;
        size_t Bytes() const override;
        llvm::Type* GenLLVM(Builder&) const override;

        std::vector<TypePtr> ElementTypes;
    };

    struct ObjectType : Type
    {
        static std::string GenString(const std::map<std::string, TypePtr>&);

        explicit ObjectType(const std::map<std::string, TypePtr>&);

        bool IsObject() const override;

        TypePtr Member(const std::string&) override;
        size_t MemberIndex(const std::string&) override;
        TypePtr Element(size_t) override;
        size_t NumElements() const override;

        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;
        size_t Bytes() const override;
        llvm::Type* GenLLVM(Builder&) const override;

        std::vector<std::pair<std::string, TypePtr>> ElementTypes;
    };

    struct FunctionType : Type
    {
        static std::string GenString(const std::vector<TypePtr>&, const TypePtr&, bool);

        FunctionType(std::vector<TypePtr>, TypePtr, bool);

        bool IsFunction() const override;

        TypePtr Result() override;

        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;
        size_t Bytes() const override;
        llvm::Type* GenLLVM(Builder&) const override;

        llvm::FunctionType* GenFnLLVM(Builder&) const;

        std::vector<TypePtr> ParamTypes;
        TypePtr ResultType;
        bool VarArg;
    };

    struct VectorType : Type
    {
        static std::string GenString(const TypePtr&);

        explicit VectorType(TypePtr);

        bool IsVector() const override;

        TypePtr Element() override;
        TypePtr Element(size_t) override;

        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;
        size_t Bytes() const override;
        llvm::Type* GenLLVM(Builder&) const override;

        static llvm::StructType* GenVecLLVM(const Builder&);

        TypePtr ElementType;
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
