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

    TypePtr GetHigherOrderOf(TypeContext &type_context, const TypePtr &type_a, const TypePtr &type_b);

    class Type
    {
    public:
        virtual ~Type() = default;

        std::ostream &Print(std::ostream &stream) const;

        [[nodiscard]] std::string GetString() const;

        template<typename T = llvm::Type>
        T *GetLLVM(const SourceLocation &where, const Builder &builder)
        {
            if (m_LLVM)
                return llvm::dyn_cast<T>(m_LLVM);
            return llvm::dyn_cast<T>(m_LLVM = GenLLVM(where, builder));
        }

        unsigned GetSize();

        [[nodiscard]] virtual bool IsPrimitive() const;
        [[nodiscard]] virtual bool IsVoid() const;
        [[nodiscard]] virtual bool IsInteger() const;
        [[nodiscard]] virtual bool IsFloatingPoint() const;
        [[nodiscard]] virtual bool IsPointer() const;
        [[nodiscard]] virtual bool IsReference() const;
        [[nodiscard]] virtual bool IsArray() const;
        [[nodiscard]] virtual bool IsStruct() const;
        [[nodiscard]] virtual bool IsTuple() const;
        [[nodiscard]] virtual bool IsFunction() const;
        [[nodiscard]] virtual bool IsSigned() const;
        [[nodiscard]] virtual unsigned GetBits() const;
        [[nodiscard]] virtual TypePtr GetElement() const;
        [[nodiscard]] virtual TypePtr GetElement(unsigned index) const;
        [[nodiscard]] virtual MemberT GetMember(const std::string_view &name) const;
        [[nodiscard]] virtual TypePtr GetResultType() const;

        virtual void TypeInfo(
            const SourceLocation &where,
            Builder &builder,
            std::vector<llvm::Value *> &args) const = 0;

    protected:
        Type(TypeContext &type_context, std::string_view string);

        [[nodiscard]] virtual llvm::Type *GenLLVM(const SourceLocation &where, const Builder &builder) const = 0;
        [[nodiscard]] virtual unsigned GenSize() const = 0;

        TypeContext &m_TypeContext;
        std::string m_String;
        llvm::Type *m_LLVM;
        unsigned m_Size;
    };

    class NoType final : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(const std::string_view &name);

        [[nodiscard]] TypePtr GetResultType() const override;
        void TypeInfo(const SourceLocation &where, Builder &builder, std::vector<llvm::Value *> &args) const override;

    private:
        NoType(TypeContext &type_context, std::string_view string, std::string_view name);

        [[nodiscard]] llvm::Type *GenLLVM(const SourceLocation &where, const Builder &builder) const override;
        [[nodiscard]] unsigned GenSize() const override;

        std::string m_Name;
    };

    class VoidType final : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString();

        [[nodiscard]] bool IsPrimitive() const override;
        [[nodiscard]] bool IsVoid() const override;
        void TypeInfo(const SourceLocation &where, Builder &builder, std::vector<llvm::Value *> &args) const override;

    private:
        VoidType(TypeContext &type_context, std::string_view string);

        [[nodiscard]] llvm::Type *GenLLVM(const SourceLocation &where, const Builder &builder) const override;
        [[nodiscard]] unsigned GenSize() const override;
    };

    class IntegerType final : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(unsigned bits, bool is_signed);

        [[nodiscard]] bool IsPrimitive() const override;
        [[nodiscard]] bool IsInteger() const override;
        [[nodiscard]] bool IsSigned() const override;
        [[nodiscard]] unsigned GetBits() const override;
        void TypeInfo(const SourceLocation &where, Builder &builder, std::vector<llvm::Value *> &args) const override;

    private:
        IntegerType(TypeContext &type_context, std::string_view string, unsigned bits, bool is_signed);

        [[nodiscard]] llvm::Type *GenLLVM(const SourceLocation &where, const Builder &builder) const override;
        [[nodiscard]] unsigned GenSize() const override;

        unsigned m_Bits;
        bool m_IsSigned;
    };

    class FloatingPointType final : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(unsigned bits);

        [[nodiscard]] bool IsPrimitive() const override;
        [[nodiscard]] bool IsFloatingPoint() const override;
        [[nodiscard]] unsigned GetBits() const override;
        void TypeInfo(const SourceLocation &where, Builder &builder, std::vector<llvm::Value *> &args) const override;

    private:
        FloatingPointType(TypeContext &type_context, std::string_view string, unsigned bits);

        [[nodiscard]] llvm::Type *GenLLVM(const SourceLocation &where, const Builder &builder) const override;
        [[nodiscard]] unsigned GenSize() const override;

        unsigned m_Bits;
    };

    class PointerType final : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(const TypePtr &element_type);

        [[nodiscard]] bool IsPrimitive() const override;
        [[nodiscard]] bool IsPointer() const override;
        [[nodiscard]] TypePtr GetElement() const override;
        void TypeInfo(const SourceLocation &where, Builder &builder, std::vector<llvm::Value *> &args) const override;

    private:
        PointerType(TypeContext &type_context, std::string_view string, TypePtr element_type);

        [[nodiscard]] llvm::Type *GenLLVM(const SourceLocation &where, const Builder &builder) const override;
        [[nodiscard]] unsigned GenSize() const override;

        TypePtr m_ElementType;
    };

    class ReferenceType final : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(const TypePtr &element_type);

        [[nodiscard]] bool IsPrimitive() const override;
        [[nodiscard]] bool IsReference() const override;
        [[nodiscard]] TypePtr GetElement() const override;
        void TypeInfo(const SourceLocation &where, Builder &builder, std::vector<llvm::Value *> &args) const override;

    private:
        ReferenceType(TypeContext &type_context, std::string_view string, TypePtr element_type);

        [[nodiscard]] llvm::Type *GenLLVM(const SourceLocation &where, const Builder &builder) const override;
        [[nodiscard]] unsigned GenSize() const override;

        TypePtr m_ElementType;
    };

    class ArrayType final : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(const TypePtr &element_type, unsigned count);

        [[nodiscard]] bool IsArray() const override;
        [[nodiscard]] TypePtr GetElement() const override;
        [[nodiscard]] TypePtr GetElement(unsigned) const override;
        void TypeInfo(const SourceLocation &where, Builder &builder, std::vector<llvm::Value *> &args) const override;

    private:
        ArrayType(TypeContext &type_context, std::string_view string, TypePtr element_type, unsigned count);

        [[nodiscard]] llvm::Type *GenLLVM(const SourceLocation &where, const Builder &builder) const override;
        [[nodiscard]] unsigned GenSize() const override;

        TypePtr m_ElementType;
        unsigned m_Count;
    };

    class StructType final : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(const std::map<std::string, TypePtr> &element_type_map);

        [[nodiscard]] bool IsStruct() const override;
        [[nodiscard]] MemberT GetMember(const std::string_view &name) const override;
        void TypeInfo(const SourceLocation &where, Builder &builder, std::vector<llvm::Value *> &args) const override;

    private:
        StructType(TypeContext &type_context, std::string_view string, std::map<std::string, TypePtr> element_type_map);

        [[nodiscard]] llvm::Type *GenLLVM(const SourceLocation &where, const Builder &builder) const override;
        [[nodiscard]] unsigned GenSize() const override;

        std::map<std::string, TypePtr> m_ElementTypeMap;
    };

    class TupleType final : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(const std::vector<TypePtr> &element_types);

        [[nodiscard]] bool IsTuple() const override;
        [[nodiscard]] TypePtr GetElement(unsigned index) const override;
        void TypeInfo(const SourceLocation &where, Builder &builder, std::vector<llvm::Value *> &args) const override;

    private:
        TupleType(TypeContext &type_context, std::string_view string, std::vector<TypePtr> element_types);

        [[nodiscard]] llvm::Type *GenLLVM(const SourceLocation &where, const Builder &builder) const override;
        [[nodiscard]] unsigned GenSize() const override;

        std::vector<TypePtr> m_ElementTypes;
    };

    class FunctionType final : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(
            const TypePtr &result_type,
            const std::vector<TypePtr> &parameter_types,
            bool var_arg);

        [[nodiscard]] bool IsPrimitive() const override;
        [[nodiscard]] bool IsFunction() const override;
        [[nodiscard]] TypePtr GetResultType() const override;
        [[nodiscard]] TypePtr ParameterType(unsigned index) const;
        [[nodiscard]] unsigned GetParameterCount() const;
        [[nodiscard]] bool IsVarArg() const;
        void TypeInfo(const SourceLocation &where, Builder &builder, std::vector<llvm::Value *> &args) const override;

        [[nodiscard]] llvm::FunctionType *GenFnLLVM(const SourceLocation &where, const Builder &builder) const;

    private:
        FunctionType(
            TypeContext &type_context,
            std::string_view string,
            TypePtr result_type,
            std::vector<TypePtr> parameter_types,
            bool var_arg);

        [[nodiscard]] llvm::Type *GenLLVM(const SourceLocation &where, const Builder &builder) const override;
        [[nodiscard]] unsigned GenSize() const override;

        TypePtr m_ResultType;
        std::vector<TypePtr> m_ParameterTypes;
        bool m_VarArg;
    };
}
