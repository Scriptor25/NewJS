#pragma once

#include <llvm/IR/Value.h>
#include <newjs/newjs.hpp>

namespace NJS
{
    class Value
    {
    public:
        virtual ~Value() = default;

        [[nodiscard]] Builder &GetBuilder() const;
        [[nodiscard]] TypePtr GetType() const;

        [[nodiscard]] virtual bool IsLValue() const = 0;
        [[nodiscard]] virtual bool IsConst() const = 0;
        [[nodiscard]] virtual llvm::Value *GetPtr(const SourceLocation &where) const = 0;

        [[nodiscard]] virtual llvm::Value *Load(const SourceLocation &where) const = 0;
        virtual void Store(const SourceLocation &where, llvm::Value *value) const = 0;
        virtual void Store(const SourceLocation &where, ValuePtr value) const = 0;
        virtual void StoreForce(const SourceLocation &where, ValuePtr value) const = 0;

    protected:
        Value(Builder &builder, TypePtr type);

    private:
        Builder &m_Builder;
        TypePtr m_Type;
    };

    class RValue final : public Value
    {
    public:
        static ValuePtr Create(Builder &builder, const TypePtr &type, llvm::Value *value);

        [[nodiscard]] bool IsLValue() const override;
        [[nodiscard]] bool IsConst() const override;
        [[nodiscard]] llvm::Value *GetPtr(const SourceLocation &where) const override;

        [[nodiscard]] llvm::Value *Load(const SourceLocation &where) const override;
        void Store(const SourceLocation &where, llvm::Value *value) const override;
        void Store(const SourceLocation &where, ValuePtr value) const override;
        void StoreForce(const SourceLocation &where, ValuePtr value) const override;

    private:
        RValue(Builder &builder, TypePtr type, llvm::Value *value);

        llvm::Value *m_Value;
    };

    class LValue final : public Value
    {
    public:
        static ValuePtr Create(Builder &builder, const TypePtr &type, llvm::Value *ptr, bool is_const);

        [[nodiscard]] bool IsLValue() const override;
        [[nodiscard]] bool IsConst() const override;
        [[nodiscard]] llvm::Value *GetPtr(const SourceLocation &where) const override;

        [[nodiscard]] llvm::Value *Load(const SourceLocation &where) const override;
        void Store(const SourceLocation &where, llvm::Value *value) const override;
        void Store(const SourceLocation &where, ValuePtr value) const override;
        void StoreForce(const SourceLocation &where, ValuePtr value) const override;

    private:
        LValue(Builder &builder, TypePtr type, llvm::Value *ptr, bool is_const);

        llvm::Value *m_Ptr;
        bool m_IsConst;
    };
}
