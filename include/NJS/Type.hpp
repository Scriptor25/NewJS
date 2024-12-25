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
    typedef std::pair<TypePtr, unsigned> MemberT;

    TypePtr max(TypeContext&, const TypePtr&, const TypePtr&);

    class Type
    {
        friend TypeContext;

    public:
        virtual ~Type() = default;

        std::ostream& Print(std::ostream&) const;

        [[nodiscard]] std::string GetString() const;

        template <typename T = llvm::Type>
        T* GetLLVM(const Builder& builder)
        {
            if (m_LLVM)
                return llvm::dyn_cast<T>(m_LLVM);
            return llvm::dyn_cast<T>(m_LLVM = GenLLVM(builder));
        }

        unsigned GetSize();

        [[nodiscard]] virtual bool IsPrimitive() const;
        [[nodiscard]] virtual bool IsVoid() const;
        [[nodiscard]] virtual bool IsInt() const;
        [[nodiscard]] virtual bool IsFP() const;
        [[nodiscard]] virtual bool IsPtr() const;
        [[nodiscard]] virtual bool IsRef() const;
        [[nodiscard]] virtual bool IsArray() const;
        [[nodiscard]] virtual bool IsStruct() const;
        [[nodiscard]] virtual bool IsTuple() const;
        [[nodiscard]] virtual bool IsFunction() const;
        [[nodiscard]] virtual bool IsSigned() const;
        [[nodiscard]] virtual unsigned GetBits() const;
        [[nodiscard]] virtual TypePtr GetElement() const;
        [[nodiscard]] virtual TypePtr GetElement(unsigned) const;
        [[nodiscard]] virtual MemberT GetMember(const std::string&) const;
        [[nodiscard]] virtual TypePtr GetResult() const;

        virtual void TypeInfo(Builder&, std::vector<llvm::Value*>&) const = 0;

    protected:
        Type(TypeContext&, std::string);

        [[nodiscard]] virtual llvm::Type* GenLLVM(const Builder&) const = 0;
        [[nodiscard]] virtual unsigned GenSize() const = 0;

        TypeContext& m_Ctx;
        std::string m_String;
        llvm::Type* m_LLVM;
        unsigned m_Size;
    };

    class NoType : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString();

        [[nodiscard]] TypePtr GetResult() const override;
        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;

    protected:
        NoType(TypeContext&, std::string);

        [[nodiscard]] llvm::Type* GenLLVM(const Builder&) const override;
        [[nodiscard]] unsigned GenSize() const override;
    };

    class VoidType : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString();

        [[nodiscard]] bool IsPrimitive() const override;
        [[nodiscard]] bool IsVoid() const override;
        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;

    protected:
        VoidType(TypeContext&, std::string);

        [[nodiscard]] llvm::Type* GenLLVM(const Builder&) const override;
        [[nodiscard]] unsigned GenSize() const override;
    };

    class IntType : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(unsigned, bool);

        [[nodiscard]] bool IsPrimitive() const override;
        [[nodiscard]] bool IsInt() const override;
        [[nodiscard]] bool IsSigned() const override;
        [[nodiscard]] unsigned GetBits() const override;
        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;

    protected:
        IntType(TypeContext&, std::string, unsigned, bool);

        [[nodiscard]] llvm::Type* GenLLVM(const Builder&) const override;
        [[nodiscard]] unsigned GenSize() const override;

        unsigned m_Bits;
        bool m_IsSigned;
    };

    class FPType : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(unsigned);

        [[nodiscard]] bool IsPrimitive() const override;
        [[nodiscard]] bool IsFP() const override;
        [[nodiscard]] unsigned GetBits() const override;
        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;

    protected:
        FPType(TypeContext&, std::string, unsigned);

        [[nodiscard]] llvm::Type* GenLLVM(const Builder&) const override;
        [[nodiscard]] unsigned GenSize() const override;

        unsigned m_Bits;
    };

    class PtrType : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(const TypePtr&);

        [[nodiscard]] bool IsPrimitive() const override;
        [[nodiscard]] bool IsPtr() const override;
        [[nodiscard]] TypePtr GetElement() const override;
        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;

    protected:
        PtrType(TypeContext&, std::string, TypePtr);

        [[nodiscard]] llvm::Type* GenLLVM(const Builder&) const override;
        [[nodiscard]] unsigned GenSize() const override;

        TypePtr m_Element;
    };

    class RefType : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(const TypePtr&);

        [[nodiscard]] bool IsPrimitive() const override;
        [[nodiscard]] bool IsRef() const override;
        [[nodiscard]] TypePtr GetElement() const override;
        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;

    protected:
        RefType(TypeContext&, std::string, TypePtr);

        [[nodiscard]] llvm::Type* GenLLVM(const Builder&) const override;
        [[nodiscard]] unsigned GenSize() const override;

        TypePtr m_Element;
    };

    class ArrayType : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(const TypePtr&, unsigned);

        [[nodiscard]] bool IsArray() const override;
        [[nodiscard]] TypePtr GetElement() const override;
        [[nodiscard]] TypePtr GetElement(unsigned) const override;
        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;

    protected:
        ArrayType(TypeContext&, std::string, TypePtr, unsigned);

        [[nodiscard]] llvm::Type* GenLLVM(const Builder&) const override;
        [[nodiscard]] unsigned GenSize() const override;

        TypePtr m_Element;
        unsigned m_Count;
    };

    class StructType : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(const std::map<std::string, TypePtr>&);

        [[nodiscard]] bool IsStruct() const override;
        [[nodiscard]] MemberT GetMember(const std::string&) const override;
        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;

    protected:
        StructType(TypeContext&, std::string, std::map<std::string, TypePtr>);

        [[nodiscard]] llvm::Type* GenLLVM(const Builder&) const override;
        [[nodiscard]] unsigned GenSize() const override;

        std::map<std::string, TypePtr> m_Elements;
    };

    class TupleType : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(const std::vector<TypePtr>&);

        [[nodiscard]] bool IsTuple() const override;
        [[nodiscard]] TypePtr GetElement() const override;
        [[nodiscard]] TypePtr GetElement(unsigned) const override;
        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;

    protected:
        TupleType(TypeContext&, std::string, std::vector<TypePtr>);

        [[nodiscard]] llvm::Type* GenLLVM(const Builder&) const override;
        [[nodiscard]] unsigned GenSize() const override;

        std::vector<TypePtr> m_Elements;
    };

    class FunctionType : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(const TypePtr&, const std::vector<TypePtr>&, bool);

        [[nodiscard]] bool IsPrimitive() const override;
        [[nodiscard]] bool IsFunction() const override;
        [[nodiscard]] TypePtr GetResult() const override;
        [[nodiscard]] TypePtr Arg(unsigned) const;
        [[nodiscard]] bool VarArg() const;
        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;

        [[nodiscard]] llvm::FunctionType* GenFnLLVM(const Builder&) const;

    protected:
        FunctionType(TypeContext&, std::string, TypePtr, std::vector<TypePtr>, bool);

        [[nodiscard]] llvm::Type* GenLLVM(const Builder&) const override;
        [[nodiscard]] unsigned GenSize() const override;

        TypePtr m_Result;
        std::vector<TypePtr> m_Args;
        bool m_VarArg;
    };
}
