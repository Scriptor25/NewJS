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

        std::string GetString() const;

        template <typename T = llvm::Type>
        T* GetLLVM(const Builder& builder)
        {
            if (m_LLVM)
                return llvm::dyn_cast<T>(m_LLVM);
            return llvm::dyn_cast<T>(m_LLVM = GenLLVM(builder));
        }

        unsigned GetSize() const;

        virtual bool IsPrimitive() const;
        virtual bool IsVoid() const;
        virtual bool IsInt() const;
        virtual bool IsFP() const;
        virtual bool IsPointer() const;
        virtual bool IsArray() const;
        virtual bool IsStruct() const;
        virtual bool IsTuple() const;
        virtual bool IsFunction() const;
        virtual bool IsSigned() const;
        virtual unsigned GetBits() const;
        virtual TypePtr GetElement() const;
        virtual TypePtr GetElement(unsigned) const;
        virtual MemberT GetMember(const std::string&) const;
        virtual TypePtr GetResult() const;

        virtual void TypeInfo(Builder&, std::vector<llvm::Value*>&) const = 0;

    protected:
        Type(TypeContext&, std::string);

        virtual llvm::Type* GenLLVM(const Builder&) const = 0;
        virtual unsigned GenSize() const = 0;

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

        TypePtr GetResult() const override;
        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;

    protected:
        NoType(TypeContext&, std::string);

        llvm::Type* GenLLVM(const Builder&) const override;
        unsigned GenSize() const override;
    };

    class VoidType : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString();

        bool IsPrimitive() const override;
        bool IsVoid() const override;
        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;

    protected:
        VoidType(TypeContext&, std::string);

        llvm::Type* GenLLVM(const Builder&) const override;
        unsigned GenSize() const override;
    };

    class IntType : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(unsigned, bool);

        bool IsPrimitive() const override;
        bool IsInt() const override;
        bool IsSigned() const override;
        unsigned GetBits() const override;
        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;

    protected:
        IntType(TypeContext&, std::string, unsigned, bool);

        llvm::Type* GenLLVM(const Builder&) const override;
        unsigned GenSize() const override;

        unsigned m_Bits;
        bool m_IsSigned;
    };

    class FPType : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(unsigned);

        bool IsPrimitive() const override;
        bool IsFP() const override;
        unsigned GetBits() const override;
        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;

    protected:
        FPType(TypeContext&, std::string, unsigned);

        llvm::Type* GenLLVM(const Builder&) const override;
        unsigned GenSize() const override;

        unsigned m_Bits;
    };

    class PointerType : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(const TypePtr&);

        bool IsPrimitive() const override;
        [[nodiscard]] bool IsPointer() const override;
        [[nodiscard]] TypePtr GetElement() const override;
        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;

    protected:
        PointerType(TypeContext&, std::string, TypePtr);

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
        TypePtr GetElement() const override;
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

        llvm::Type* GenLLVM(const Builder&) const override;
        unsigned GenSize() const override;

        std::map<std::string, TypePtr> m_Elements;
    };

    class TupleType : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(const std::vector<TypePtr>&);

        bool IsTuple() const override;
        TypePtr GetElement() const override;
        TypePtr GetElement(unsigned) const override;
        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;

    protected:
        TupleType(TypeContext&, std::string, std::vector<TypePtr>);

        llvm::Type* GenLLVM(const Builder&) const override;
        unsigned GenSize() const override;

        std::vector<TypePtr> m_Elements;
    };

    class FunctionType : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(const TypePtr&, const std::vector<TypePtr>&, bool);

        bool IsPrimitive() const override;
        bool IsFunction() const override;
        TypePtr GetResult() const override;
        TypePtr Arg(unsigned) const;
        bool VarArg() const;
        void TypeInfo(Builder&, std::vector<llvm::Value*>&) const override;

        llvm::FunctionType* GenFnLLVM(const Builder&) const;

    protected:
        FunctionType(TypeContext&, std::string, TypePtr, std::vector<TypePtr>, bool);

        llvm::Type* GenLLVM(const Builder&) const override;
        unsigned GenSize() const override;

        TypePtr m_Result;
        std::vector<TypePtr> m_Args;
        bool m_VarArg;
    };
}
