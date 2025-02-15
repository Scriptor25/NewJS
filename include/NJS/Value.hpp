#pragma once

#include <llvm/IR/Value.h>
#include <NJS/NJS.hpp>

namespace NJS
{
    class Value
    {
    public:
        virtual ~Value() = default;

        [[nodiscard]] Builder &GetBuilder() const;
        [[nodiscard]] TypePtr GetType() const;

        [[nodiscard]] virtual bool IsLValue() const = 0;
        [[nodiscard]] virtual llvm::Value *GetPtr(const SourceLocation &where) const = 0;

        [[nodiscard]] virtual llvm::Value *Load(const SourceLocation &where) const = 0;
        virtual void Store(const SourceLocation &where, llvm::Value *value, bool force = false) const = 0;
        virtual void Store(const SourceLocation &where, ValuePtr value) const = 0;

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
        [[nodiscard]] llvm::Value *GetPtr(const SourceLocation &where) const override;

        [[nodiscard]] llvm::Value *Load(const SourceLocation &where) const override;
        void Store(const SourceLocation &where, llvm::Value *value, bool force = false) const override;
        void Store(const SourceLocation &where, ValuePtr value) const override;

    private:
        RValue(Builder &builder, TypePtr type, llvm::Value *value);

        llvm::Value *m_Value;
    };

    class LValue final : public Value
    {
    public:
        static ValuePtr Create(Builder &builder, const TypePtr &type, llvm::Value *ptr);

        [[nodiscard]] bool IsLValue() const override;
        [[nodiscard]] llvm::Value *GetPtr(const SourceLocation &where) const override;

        [[nodiscard]] llvm::Value *Load(const SourceLocation &where) const override;
        void Store(const SourceLocation &where, llvm::Value *value, bool force = false) const override;
        void Store(const SourceLocation &where, ValuePtr value) const override;

    private:
        LValue(Builder &builder, TypePtr type, llvm::Value *ptr);

        llvm::Value *m_Ptr;
    };
}
