#pragma once

#include <memory>
#include <string>
#include <vector>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <NJS/Info.hpp>
#include <NJS/NJS.hpp>

namespace NJS
{
    TypePtr GetHigherOrderOf(
        const SourceLocation &where,
        TypeContext &type_context,
        const TypePtr &type_a,
        const TypePtr &type_b);

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

        unsigned GetSize(const SourceLocation &where, const Builder &builder);

        [[nodiscard]] virtual bool IsPrimitive() const;
        [[nodiscard]] virtual bool IsIncomplete() const;
        [[nodiscard]] virtual bool IsVoid() const;
        [[nodiscard]] virtual bool IsInteger() const;
        [[nodiscard]] virtual bool IsFloatingPoint() const;
        [[nodiscard]] virtual bool IsPointer() const;
        [[nodiscard]] virtual bool IsArray() const;
        [[nodiscard]] virtual bool IsStruct() const;
        [[nodiscard]] virtual bool IsTuple() const;
        [[nodiscard]] virtual bool IsFunction() const;

        [[nodiscard]] virtual bool IsSigned(const SourceLocation &where) const;
        [[nodiscard]] virtual unsigned GetBits(const SourceLocation &where) const;

        [[nodiscard]] virtual TypePtr GetElement(const SourceLocation &where) const;
        [[nodiscard]] virtual TypePtr GetElement(const SourceLocation &where, unsigned index) const;
        [[nodiscard]] virtual unsigned GetElementCount(const SourceLocation &where) const;

        [[nodiscard]] virtual MemberInfo GetMember(const SourceLocation &where, const std::string &name) const;
        [[nodiscard]] virtual MemberInfo GetMember(const SourceLocation &where, unsigned index) const;

        [[nodiscard]] virtual ReferenceInfo GetResult(const SourceLocation &where) const;
        [[nodiscard]] virtual ReferenceInfo GetParameter(const SourceLocation &where, unsigned index) const;
        [[nodiscard]] virtual unsigned GetParameterCount(const SourceLocation &where) const;
        [[nodiscard]] virtual bool IsVarArg(const SourceLocation &where) const;

        virtual bool TypeInfo(
            const SourceLocation &where,
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const = 0;

    protected:
        Type(TypeContext &type_context, std::string string);

        [[nodiscard]] virtual llvm::Type *GenLLVM(const SourceLocation &where, const Builder &builder) const = 0;

        TypeContext &m_TypeContext;
        std::string m_String;
        llvm::Type *m_LLVM;
        unsigned m_Size;
    };

    class IncompleteType final : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(const std::string &name);

        bool IsIncomplete() const override;

        bool TypeInfo(
            const SourceLocation &where,
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const override;

    private:
        IncompleteType(TypeContext &type_context, std::string string, std::string name);

        [[nodiscard]] llvm::Type *GenLLVM(const SourceLocation &where, const Builder &builder) const override;

        std::string m_Name;
    };

    class VoidType final : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString();

        [[nodiscard]] bool IsPrimitive() const override;
        [[nodiscard]] bool IsVoid() const override;

        bool TypeInfo(
            const SourceLocation &where,
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const override;

    private:
        VoidType(TypeContext &type_context, std::string string);

        [[nodiscard]] llvm::Type *GenLLVM(const SourceLocation &where, const Builder &builder) const override;
    };

    class IntegerType final : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(unsigned bits, bool is_signed);

        [[nodiscard]] bool IsPrimitive() const override;
        [[nodiscard]] bool IsInteger() const override;
        [[nodiscard]] bool IsSigned(const SourceLocation &where) const override;
        [[nodiscard]] unsigned GetBits(const SourceLocation &where) const override;

        bool TypeInfo(
            const SourceLocation &where,
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const override;

    private:
        IntegerType(TypeContext &type_context, std::string string, unsigned bits, bool is_signed);

        [[nodiscard]] llvm::Type *GenLLVM(const SourceLocation &where, const Builder &builder) const override;

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
        [[nodiscard]] unsigned GetBits(const SourceLocation &where) const override;

        bool TypeInfo(
            const SourceLocation &where,
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const override;

    private:
        FloatingPointType(TypeContext &type_context, std::string string, unsigned bits);

        [[nodiscard]] llvm::Type *GenLLVM(const SourceLocation &where, const Builder &builder) const override;

        unsigned m_Bits;
    };

    class PointerType final : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(const TypePtr &element_type);

        [[nodiscard]] bool IsPrimitive() const override;
        [[nodiscard]] bool IsPointer() const override;
        [[nodiscard]] TypePtr GetElement(const SourceLocation &where) const override;
        [[nodiscard]] TypePtr GetElement(const SourceLocation &where, unsigned index) const override;
        [[nodiscard]] unsigned GetElementCount(const SourceLocation &where) const override;

        bool TypeInfo(
            const SourceLocation &where,
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const override;

    private:
        PointerType(TypeContext &type_context, std::string string, TypePtr element_type);

        [[nodiscard]] llvm::Type *GenLLVM(const SourceLocation &where, const Builder &builder) const override;

        TypePtr m_ElementType;
    };

    class ArrayType final : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(const TypePtr &element_type, unsigned count);

        [[nodiscard]] bool IsArray() const override;
        [[nodiscard]] TypePtr GetElement(const SourceLocation &where) const override;
        [[nodiscard]] TypePtr GetElement(const SourceLocation &where, unsigned index) const override;
        [[nodiscard]] unsigned GetElementCount(const SourceLocation &where) const override;

        bool TypeInfo(
            const SourceLocation &where,
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const override;

    private:
        ArrayType(TypeContext &type_context, std::string string, TypePtr element_type, unsigned count);

        [[nodiscard]] llvm::Type *GenLLVM(const SourceLocation &where, const Builder &builder) const override;

        TypePtr m_ElementType;
        unsigned m_Count;
    };

    class StructType final : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(const std::vector<std::pair<std::string, TypePtr>> &element_types);

        [[nodiscard]] bool IsStruct() const override;
        [[nodiscard]] unsigned GetElementCount(const SourceLocation &where) const override;
        [[nodiscard]] MemberInfo GetMember(const SourceLocation &where, const std::string &name) const override;
        [[nodiscard]] MemberInfo GetMember(const SourceLocation &where, unsigned index) const override;

        bool TypeInfo(
            const SourceLocation &where,
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const override;

    private:
        StructType(
            TypeContext &type_context,
            std::string string,
            std::vector<std::pair<std::string, TypePtr>> element_types);

        [[nodiscard]] llvm::Type *GenLLVM(const SourceLocation &where, const Builder &builder) const override;

        std::vector<std::pair<std::string, TypePtr>> m_ElementTypes;
        unsigned m_Index;
    };

    class TupleType final : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(const std::vector<TypePtr> &element_types);

        [[nodiscard]] bool IsTuple() const override;
        [[nodiscard]] TypePtr GetElement(const SourceLocation &where, unsigned index) const override;
        [[nodiscard]] unsigned GetElementCount(const SourceLocation &where) const override;

        bool TypeInfo(
            const SourceLocation &where,
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const override;

    private:
        TupleType(TypeContext &type_context, std::string string, std::vector<TypePtr> element_types);

        [[nodiscard]] llvm::Type *GenLLVM(const SourceLocation &where, const Builder &builder) const override;

        std::vector<TypePtr> m_ElementTypes;
        unsigned m_Index;
    };

    class FunctionType final : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(
            const ReferenceInfo &result,
            const std::vector<ReferenceInfo> &parameters,
            bool is_var_arg);

        [[nodiscard]] bool IsPrimitive() const override;
        [[nodiscard]] bool IsFunction() const override;

        [[nodiscard]] ReferenceInfo GetResult(const SourceLocation &where) const override;
        [[nodiscard]] ReferenceInfo GetParameter(const SourceLocation &where, unsigned index) const override;
        [[nodiscard]] unsigned GetParameterCount(const SourceLocation &where) const override;
        [[nodiscard]] bool IsVarArg(const SourceLocation &where) const override;

        bool TypeInfo(
            const SourceLocation &where,
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const override;

        [[nodiscard]] llvm::FunctionType *GenFnLLVM(const SourceLocation &where, const Builder &builder) const;

    private:
        FunctionType(
            TypeContext &type_context,
            std::string string,
            ReferenceInfo result,
            std::vector<ReferenceInfo> parameters,
            bool is_var_arg);

        [[nodiscard]] llvm::Type *GenLLVM(const SourceLocation &where, const Builder &builder) const override;

        ReferenceInfo m_Result;
        std::vector<ReferenceInfo> m_Parameters;
        bool m_IsVarArg;
    };
}
