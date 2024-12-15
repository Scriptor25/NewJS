#pragma once

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
    typedef std::pair<TypePtr, size_t> MemberT;

    struct Type
    {
        Type(TypeContext&, std::string);
        virtual ~Type() = default;

        [[nodiscard]] virtual bool IsNo() const;
        [[nodiscard]] virtual bool IsPrimitive() const;
        [[nodiscard]] virtual bool IsPrimitive(Primitive) const;
        [[nodiscard]] virtual bool IsArray() const;
        [[nodiscard]] virtual bool IsTuple() const;
        [[nodiscard]] virtual bool IsObject() const;
        [[nodiscard]] virtual bool IsFunction() const;
        [[nodiscard]] virtual bool IsVector() const;

        [[nodiscard]] virtual MemberT Member(const std::string&) const;
        [[nodiscard]] virtual TypePtr Result() const;
        [[nodiscard]] virtual TypePtr Element() const;
        [[nodiscard]] virtual TypePtr Element(size_t) const;
        [[nodiscard]] virtual size_t NumElements() const;

        virtual void TypeInfo(Builder&, std::vector<llvm::Value*>&) const = 0;
        [[nodiscard]] virtual size_t Bytes() const = 0;
        [[nodiscard]] virtual llvm::Type* GenLLVM(Builder&) const = 0;

        std::ostream& Print(std::ostream&) const;

        TypeContext& Ctx;
        std::string String;
    };

    struct NoType : Type
    {
        static std::string GenString();

        explicit NoType(TypeContext&);

        [[nodiscard]] bool IsNo() const override;

        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;
        [[nodiscard]] size_t Bytes() const override;
        [[nodiscard]] llvm::Type* GenLLVM(Builder&) const override;
    };

    struct PrimitiveType : Type
    {
        static std::string GenString(Primitive);

        PrimitiveType(TypeContext&, Primitive);

        [[nodiscard]] bool IsPrimitive() const override;
        [[nodiscard]] bool IsPrimitive(Primitive) const override;

        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;
        [[nodiscard]] size_t Bytes() const override;
        [[nodiscard]] llvm::Type* GenLLVM(Builder&) const override;

        Primitive Name;
    };

    struct ArrayType : Type
    {
        static std::string GenString(const TypePtr&, size_t);

        ArrayType(TypeContext&, TypePtr, size_t);

        [[nodiscard]] bool IsArray() const override;

        [[nodiscard]] TypePtr Element() const override;
        [[nodiscard]] TypePtr Element(size_t) const override;
        [[nodiscard]] size_t NumElements() const override;

        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;
        [[nodiscard]] size_t Bytes() const override;
        [[nodiscard]] llvm::Type* GenLLVM(Builder&) const override;

        TypePtr ElementType;
        size_t ElementCount;
    };

    struct TupleType : Type
    {
        static std::string GenString(const std::vector<TypePtr>&);

        TupleType(TypeContext&, std::vector<TypePtr>);

        [[nodiscard]] bool IsTuple() const override;

        [[nodiscard]] TypePtr Element(size_t) const override;
        [[nodiscard]] size_t NumElements() const override;

        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;
        [[nodiscard]] size_t Bytes() const override;
        [[nodiscard]] llvm::Type* GenLLVM(Builder&) const override;

        std::vector<TypePtr> ElementTypes;
    };

    struct ObjectType : Type
    {
        static std::string GenString(const std::map<std::string, TypePtr>&);

        ObjectType(TypeContext&, std::map<std::string, TypePtr>);

        [[nodiscard]] bool IsObject() const override;

        [[nodiscard]] MemberT Member(const std::string&) const override;
        [[nodiscard]] TypePtr Element(size_t) const override;
        [[nodiscard]] size_t NumElements() const override;

        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;
        [[nodiscard]] size_t Bytes() const override;
        [[nodiscard]] llvm::Type* GenLLVM(Builder&) const override;

        std::map<std::string, TypePtr> ElementTypes;
    };

    struct FunctionType : Type
    {
        static std::string GenString(const std::vector<TypePtr>&, const TypePtr&, bool);

        FunctionType(TypeContext&, std::vector<TypePtr>, TypePtr, bool);

        [[nodiscard]] bool IsFunction() const override;

        [[nodiscard]] TypePtr Result() const override;

        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;
        [[nodiscard]] size_t Bytes() const override;
        [[nodiscard]] llvm::Type* GenLLVM(Builder&) const override;

        llvm::FunctionType* GenFnLLVM(Builder&) const;

        std::vector<TypePtr> ParamTypes;
        TypePtr ResultType;
        bool VarArg;
    };

    struct VectorType : Type
    {
        static std::string GenString(const TypePtr&);

        VectorType(TypeContext&, TypePtr);

        [[nodiscard]] bool IsVector() const override;

        [[nodiscard]] TypePtr Element() const override;
        [[nodiscard]] TypePtr Element(size_t) const override;

        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;
        [[nodiscard]] size_t Bytes() const override;
        [[nodiscard]] llvm::Type* GenLLVM(Builder&) const override;

        static llvm::StructType* GenVecLLVM(const Builder&);

        TypePtr ElementType;
    };
}
