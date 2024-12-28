#pragma once

#include <llvm/IR/Value.h>
#include <NJS/NJS.hpp>

namespace NJS
{
    class Value
    {
    public:
        virtual ~Value() = default;

        [[nodiscard]] Builder& GetBuilder() const;
        [[nodiscard]] TypePtr GetType() const;

        [[nodiscard]] virtual bool IsL() const = 0;
        [[nodiscard]] virtual llvm::Value* GetPtr(const SourceLocation&) const = 0;

        [[nodiscard]] virtual llvm::Value* Load(const SourceLocation&) const = 0;
        virtual void Store(const SourceLocation&, llvm::Value*) const = 0;
        virtual void Store(const SourceLocation&, ValuePtr) const = 0;

    protected:
        Value(Builder&, TypePtr);

    private:
        Builder& m_Builder;
        TypePtr m_Type;
    };

    class RValue final : public Value
    {
    public:
        static ValuePtr Create(Builder&, const TypePtr&, llvm::Value*);

        [[nodiscard]] bool IsL() const override;
        [[nodiscard]] llvm::Value* GetPtr(const SourceLocation&) const override;

        [[nodiscard]] llvm::Value* Load(const SourceLocation&) const override;
        void Store(const SourceLocation&, llvm::Value*) const override;
        void Store(const SourceLocation&, ValuePtr) const override;

    private:
        RValue(Builder&, TypePtr, llvm::Value*);

        llvm::Value* m_Value;
    };

    class LValue final : public Value
    {
    public:
        static ValuePtr Create(Builder&, const TypePtr&, llvm::Value*);

        [[nodiscard]] bool IsL() const override;
        [[nodiscard]] llvm::Value* GetPtr(const SourceLocation&) const override;

        [[nodiscard]] llvm::Value* Load(const SourceLocation&) const override;
        void Store(const SourceLocation&, llvm::Value*) const override;
        void Store(const SourceLocation&, ValuePtr) const override;

    private:
        LValue(Builder&, TypePtr, llvm::Value*);

        llvm::Value* m_Ptr;
    };
}
