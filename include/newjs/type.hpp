#pragma once

#include <memory>
#include <string>
#include <vector>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <newjs/info.hpp>
#include <newjs/newjs.hpp>

namespace NJS
{
    TypePtr GetHigherOrderOf(
        TypeContext &type_context,
        const TypePtr &type_a,
        const TypePtr &type_b);

    class Type
    {
    public:
        template<typename T, typename V>
        static std::shared_ptr<T> As(std::shared_ptr<V> v)
        {
            return std::dynamic_pointer_cast<T>(v);
        }

        virtual ~Type() = default;

        std::ostream &Print(std::ostream &stream) const;

        [[nodiscard]] std::string GetString() const;

        template<typename T = llvm::Type>
        T *GetLLVM(const Builder &builder)
        {
            if (m_LLVM)
                return llvm::dyn_cast<T>(m_LLVM);
            return llvm::dyn_cast<T>(m_LLVM = GenLLVM(builder));
        }

        unsigned GetSize(const Builder &builder);

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

        virtual bool TypeInfo(
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const = 0;

    protected:
        Type(TypeContext &type_context, std::string string);

        [[nodiscard]] virtual llvm::Type *GenLLVM(const Builder &builder) const = 0;

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

        [[nodiscard]] bool IsIncomplete() const override;

        bool TypeInfo(
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const override;

    private:
        IncompleteType(TypeContext &type_context, std::string string, std::string name);

        [[nodiscard]] llvm::Type *GenLLVM(const Builder &builder) const override;

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
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const override;

    private:
        VoidType(TypeContext &type_context, std::string string);

        [[nodiscard]] llvm::Type *GenLLVM(const Builder &builder) const override;
    };

    class IntegerType final : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(unsigned bits, bool is_signed);

        [[nodiscard]] bool IsPrimitive() const override;
        [[nodiscard]] bool IsInteger() const override;
        [[nodiscard]] bool IsSigned() const;
        [[nodiscard]] unsigned GetBits() const;

        bool TypeInfo(
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const override;

    private:
        IntegerType(TypeContext &type_context, std::string string, unsigned bits, bool is_signed);

        [[nodiscard]] llvm::Type *GenLLVM(const Builder &builder) const override;

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
        [[nodiscard]] unsigned GetBits() const;

        bool TypeInfo(
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const override;

    private:
        FloatingPointType(TypeContext &type_context, std::string string, unsigned bits);

        [[nodiscard]] llvm::Type *GenLLVM(const Builder &builder) const override;

        unsigned m_Bits;
    };

    class PointerType final : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(const TypePtr &element_type, bool is_const);

        [[nodiscard]] bool IsPrimitive() const override;
        [[nodiscard]] bool IsPointer() const override;
        [[nodiscard]] bool IsConst() const;
        [[nodiscard]] TypePtr GetElement() const;

        bool TypeInfo(
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const override;

    private:
        PointerType(TypeContext &type_context, std::string string, TypePtr element_type, bool is_const);

        [[nodiscard]] llvm::Type *GenLLVM(const Builder &builder) const override;

        TypePtr m_ElementType;
        bool m_IsConst;
    };

    class ArrayType final : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(const TypePtr &element_type, unsigned count);

        [[nodiscard]] bool IsArray() const override;
        [[nodiscard]] TypePtr GetElement() const;
        [[nodiscard]] unsigned GetElementCount() const;

        bool TypeInfo(
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const override;

    private:
        ArrayType(TypeContext &type_context, std::string string, TypePtr element_type, unsigned count);

        [[nodiscard]] llvm::Type *GenLLVM(const Builder &builder) const override;

        TypePtr m_ElementType;
        unsigned m_Count;
    };

    class StructType final : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(const std::vector<std::pair<std::string, TypePtr>> &element_types);

        [[nodiscard]] bool IsStruct() const override;
        [[nodiscard]] unsigned GetElementCount() const;
        [[nodiscard]] MemberInfo GetMember(const std::string &name) const;
        [[nodiscard]] MemberInfo GetMember(unsigned index) const;

        bool TypeInfo(
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const override;

    private:
        StructType(
            TypeContext &type_context,
            std::string string,
            std::vector<std::pair<std::string, TypePtr>> element_types);

        [[nodiscard]] llvm::Type *GenLLVM(const Builder &builder) const override;

        std::vector<std::pair<std::string, TypePtr>> m_ElementTypes;
        unsigned m_Index;
    };

    class TupleType final : public Type
    {
        friend TypeContext;

    public:
        static std::string GenString(const std::vector<TypePtr> &element_types);

        [[nodiscard]] bool IsTuple() const override;
        [[nodiscard]] TypePtr GetElement(unsigned index) const;
        [[nodiscard]] unsigned GetElementCount() const;

        bool TypeInfo(
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const override;

    private:
        TupleType(TypeContext &type_context, std::string string, std::vector<TypePtr> element_types);

        [[nodiscard]] llvm::Type *GenLLVM(const Builder &builder) const override;

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

        [[nodiscard]] ReferenceInfo GetResult() const;
        [[nodiscard]] ReferenceInfo GetParameter(unsigned index) const;
        [[nodiscard]] unsigned GetParameterCount() const;
        [[nodiscard]] bool IsVarArg() const;

        bool TypeInfo(
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const override;

        [[nodiscard]] llvm::FunctionType *GenFnLLVM(const Builder &builder) const;

    private:
        FunctionType(
            TypeContext &type_context,
            std::string string,
            ReferenceInfo result,
            std::vector<ReferenceInfo> parameters,
            bool is_var_arg);

        [[nodiscard]] llvm::Type *GenLLVM(const Builder &builder) const override;

        ReferenceInfo m_Result;
        std::vector<ReferenceInfo> m_Parameters;
        bool m_IsVarArg;
    };
}
