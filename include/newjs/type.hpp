#pragma once

#include <memory>
#include <string>
#include <vector>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Value.h>
#include <newjs/info.hpp>
#include <newjs/newjs.hpp>

namespace NJS
{
    TypePtr CombineTypes(
        TypeContext &type_context,
        const TypePtr &type_a,
        const TypePtr &type_b);

    unsigned CombineHashes(unsigned h1, unsigned h2);

    class Type
    {
    public:
        static constexpr auto INFO = "Type";

        template<typename T>
        static std::shared_ptr<T> As(TypePtr v)
        {
            if (auto p = std::dynamic_pointer_cast<T>(v))
                return p;
            Error("cannot get type {} as {}", v, T::INFO);
        }

        virtual ~Type() = default;

        [[nodiscard]] TypeContext &GetContext() const;
        [[nodiscard]] unsigned GetHash() const;
        [[nodiscard]] std::string GetString() const;

        template<typename T>
        [[nodiscard]] T *GetLLVM(const Builder &builder) const
        {
            return llvm::dyn_cast<T>(GenLLVM(builder));
        }

        [[nodiscard]] llvm::Type *GetLLVM(const Builder &builder) const
        {
            return GenLLVM(builder);
        }

        unsigned GetSize(const Builder &builder) const;
        [[nodiscard]] bool IsIntegerLike() const;
        [[nodiscard]] bool IsBoolean() const;

        [[nodiscard]] virtual bool IsPrimitive() const;
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
        virtual std::ostream &Print(std::ostream &stream) const =0;

    protected:
        Type(TypeContext &type_context, unsigned hash, std::string string);

        [[nodiscard]] virtual llvm::Type *GenLLVM(const Builder &builder) const = 0;

        TypeContext &m_TypeContext;
        unsigned m_Hash;
        std::string m_String;
    };

    class VoidType final : public Type
    {
        friend TypeContext;

    public:
        static constexpr auto INFO = "VoidType";

        static std::string GenString();
        static unsigned GenHash();

        [[nodiscard]] bool IsPrimitive() const override;
        [[nodiscard]] bool IsVoid() const override;

        bool TypeInfo(
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const override;
        std::ostream &Print(std::ostream &stream) const override;

    private:
        VoidType(TypeContext &type_context, unsigned hash, std::string string);

        [[nodiscard]] llvm::Type *GenLLVM(const Builder &builder) const override;
    };

    class IntegerType final : public Type
    {
        friend TypeContext;

    public:
        static constexpr auto INFO = "IntegerType";

        static std::string GenString(unsigned bits, bool is_signed);
        static unsigned GenHash(unsigned bits, bool is_signed);

        [[nodiscard]] bool IsPrimitive() const override;
        [[nodiscard]] bool IsInteger() const override;
        [[nodiscard]] bool IsSigned() const;
        [[nodiscard]] unsigned GetBits() const;

        bool TypeInfo(
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const override;
        std::ostream &Print(std::ostream &stream) const override;

    private:
        IntegerType(TypeContext &type_context, unsigned hash, std::string string, unsigned bits, bool is_signed);

        [[nodiscard]] llvm::Type *GenLLVM(const Builder &builder) const override;

        unsigned m_Bits;
        bool m_IsSigned;
    };

    class FloatingPointType final : public Type
    {
        friend TypeContext;

    public:
        static constexpr auto INFO = "FloatingPointType";

        static std::string GenString(unsigned bits);
        static unsigned GenHash(unsigned bits);

        [[nodiscard]] bool IsPrimitive() const override;
        [[nodiscard]] bool IsFloatingPoint() const override;
        [[nodiscard]] unsigned GetBits() const;

        bool TypeInfo(
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const override;
        std::ostream &Print(std::ostream &stream) const override;

    private:
        FloatingPointType(TypeContext &type_context, unsigned hash, std::string string, unsigned bits);

        [[nodiscard]] llvm::Type *GenLLVM(const Builder &builder) const override;

        unsigned m_Bits;
    };

    class PointerType final : public Type
    {
        friend TypeContext;

    public:
        static constexpr auto INFO = "PointerType";

        static std::string GenString(const TypePtr &element_type, bool is_const);
        static unsigned GenHash(const TypePtr &element_type, bool is_const);

        [[nodiscard]] bool IsPrimitive() const override;
        [[nodiscard]] bool IsPointer() const override;
        [[nodiscard]] bool IsConst() const;
        [[nodiscard]] TypePtr GetElement() const;

        bool TypeInfo(
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const override;
        std::ostream &Print(std::ostream &stream) const override;

    private:
        PointerType(TypeContext &type_context, unsigned hash, std::string string, TypePtr element_type, bool is_const);

        [[nodiscard]] llvm::Type *GenLLVM(const Builder &builder) const override;

        TypePtr m_ElementType;
        bool m_IsConst;
    };

    class ArrayType final : public Type
    {
        friend TypeContext;

    public:
        static constexpr auto INFO = "ArrayType";

        static std::string GenString(const TypePtr &element_type, unsigned count);
        static unsigned GenHash(const TypePtr &element_type, unsigned count);

        [[nodiscard]] bool IsArray() const override;
        [[nodiscard]] TypePtr GetElement() const;
        [[nodiscard]] unsigned GetElementCount() const;

        bool TypeInfo(
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const override;
        std::ostream &Print(std::ostream &stream) const override;

    private:
        ArrayType(TypeContext &type_context, unsigned hash, std::string string, TypePtr element_type, unsigned count);

        [[nodiscard]] llvm::Type *GenLLVM(const Builder &builder) const override;

        TypePtr m_ElementType;
        unsigned m_Count;
    };

    struct StructElement
    {
        std::string Name;
        ReferenceInfo Info;
        ExpressionPtr Default;
    };

    class StructType final : public Type
    {
        friend TypeContext;

    public:
        static constexpr auto INFO = "StructType";

        static std::string GenString(const std::vector<StructElement> &elements, const std::string &name);
        static unsigned GenHash(const std::vector<StructElement> &elements, const std::string &name);

        [[nodiscard]] bool IsStruct() const override;

        [[nodiscard]] unsigned GetElementCount() const;
        [[nodiscard]] MemberInfo GetMember(const std::string &name) const;
        [[nodiscard]] MemberInfo GetMember(unsigned index) const;

        void SetElements(const std::vector<StructElement> &elements);

        bool TypeInfo(
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const override;
        std::ostream &Print(std::ostream &stream) const override;

    private:
        StructType(
            TypeContext &type_context,
            unsigned hash,
            std::string string,
            std::vector<StructElement> elements,
            std::string name);

        [[nodiscard]] llvm::Type *GenLLVM(const Builder &builder) const override;

        std::vector<StructElement> m_Elements;
        std::string m_Name;
    };

    class TupleType final : public Type
    {
        friend TypeContext;

    public:
        static constexpr auto INFO = "TupleType";

        static std::string GenString(const std::vector<TypePtr> &element_types);
        static unsigned GenHash(const std::vector<TypePtr> &element_types);

        [[nodiscard]] bool IsTuple() const override;
        [[nodiscard]] TypePtr GetElement(unsigned index) const;
        [[nodiscard]] unsigned GetElementCount() const;

        bool TypeInfo(
            Builder &builder,
            std::vector<llvm::Value *> &arguments) const override;
        std::ostream &Print(std::ostream &stream) const override;

    private:
        TupleType(TypeContext &type_context, unsigned hash, std::string string, std::vector<TypePtr> element_types);

        [[nodiscard]] llvm::Type *GenLLVM(const Builder &builder) const override;

        std::vector<TypePtr> m_ElementTypes;
    };

    class FunctionType final : public Type
    {
        friend TypeContext;

    public:
        static constexpr auto INFO = "FunctionType";

        static std::string GenString(
            const ReferenceInfo &result,
            const std::vector<ReferenceInfo> &parameters,
            bool is_var_arg);
        static unsigned GenHash(
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
        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] llvm::FunctionType *GenFnLLVM(const Builder &builder) const;

    private:
        FunctionType(
            TypeContext &type_context,
            unsigned hash,
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
