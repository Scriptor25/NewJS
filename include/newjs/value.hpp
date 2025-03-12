#pragma once

#include <newjs/newjs.hpp>

namespace NJS
{
    class Value
    {
    public:
        virtual ~Value() = default;

        [[nodiscard]] const Builder &GetBuilder() const;
        [[nodiscard]] TypePtr GetType() const;

        [[nodiscard]] virtual bool IsLValue() const = 0;
        [[nodiscard]] virtual bool IsConst() const = 0;
        [[nodiscard]] virtual llvm::Value *GetPointer() const = 0;

        [[nodiscard]] virtual llvm::Value *Load() const = 0;
        virtual void Store(llvm::Value *value) const = 0;
        virtual void Store(ValuePtr value) const = 0;
        virtual void StoreNoError(ValuePtr value) const = 0;

    protected:
        Value(const Builder &builder, TypePtr type);

    private:
        const Builder &m_Builder;
        TypePtr m_Type;
    };

    class RValue final : public Value
    {
    public:
        static ValuePtr Create(const Builder &builder, const TypePtr &type, llvm::Value *value);

        [[nodiscard]] bool IsLValue() const override;
        [[nodiscard]] bool IsConst() const override;
        [[nodiscard]] llvm::Value *GetPointer() const override;

        [[nodiscard]] llvm::Value *Load() const override;
        void Store(llvm::Value *value) const override;
        void Store(ValuePtr value) const override;
        void StoreNoError(ValuePtr value) const override;

    private:
        RValue(const Builder &builder, TypePtr type, llvm::Value *value);

        llvm::Value *m_Value;
    };

    class LValue final : public Value
    {
    public:
        static ValuePtr Create(const Builder &builder, const TypePtr &type, llvm::Value *pointer, bool is_const);

        [[nodiscard]] bool IsLValue() const override;
        [[nodiscard]] bool IsConst() const override;
        [[nodiscard]] llvm::Value *GetPointer() const override;

        [[nodiscard]] llvm::Value *Load() const override;
        void Store(llvm::Value *value) const override;
        void Store(ValuePtr value) const override;
        void StoreNoError(ValuePtr value) const override;

    private:
        LValue(const Builder &builder, TypePtr type, llvm::Value *pointer, bool is_const);

        llvm::Value *m_Pointer;
        bool m_IsConst;
    };
}
