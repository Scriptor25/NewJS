#pragma once

#include <memory>
#include <NJS/Builder.hpp>

namespace NJS
{
    class Builder;

    typedef std::shared_ptr<struct Type> TypePtr;
    typedef std::shared_ptr<class Value> ValuePtr;

    class Value
    {
    public:
        virtual ~Value() = default;

        [[nodiscard]] Builder& GetBuilder() const;
        [[nodiscard]] TypePtr GetType() const;
        [[nodiscard]] llvm::Type* GetLLVMType() const;

        [[nodiscard]] virtual bool IsL() const = 0;
        [[nodiscard]] virtual llvm::Value* GetPtr() const = 0;

        virtual llvm::Value* Load() = 0;
        virtual void Store(llvm::Value*) = 0;

    protected:
        Value(Builder&, TypePtr);

    private:
        Builder& m_Builder;
        TypePtr m_Type;
        llvm::Type* m_LLVMType;
    };

    class RValue : public Value
    {
    public:
        static ValuePtr Create(Builder&, const TypePtr&, llvm::Value*);

        [[nodiscard]] bool IsL() const override;
        [[nodiscard]] llvm::Value* GetPtr() const override;

        llvm::Value* Load() override;
        void Store(llvm::Value*) override;

    private:
        RValue(Builder&, TypePtr, llvm::Value*);

        llvm::Value* m_Value;
    };

    class LValue : public Value
    {
    public:
        static ValuePtr Create(Builder&, const TypePtr&, llvm::Value*);

        [[nodiscard]] bool IsL() const override;
        [[nodiscard]] llvm::Value* GetPtr() const override;

        llvm::Value* Load() override;
        void Store(llvm::Value*) override;

    private:
        LValue(Builder&, TypePtr, llvm::Value*);

        llvm::Value* m_Ptr;
    };
}
