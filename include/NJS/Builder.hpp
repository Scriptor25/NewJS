#pragma once

#include <map>
#include <memory>
#include <vector>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <NJS/NJS.hpp>

namespace NJS
{
    struct StackFrame
    {
        [[nodiscard]] bool contains(const std::string_view &) const;
        ValuePtr operator[](const std::string_view &) const;
        ValuePtr &operator[](const std::string_view &);

        [[nodiscard]] std::string ValueName(const std::string_view &) const;

        std::string ParentName;
        TypePtr ResultType;
        std::map<std::string, ValuePtr> Values;
    };

    struct ValueRef
    {
        llvm::Value *Val;
        llvm::Value *Ptr;
    };

    struct OperatorRef
    {
        TypePtr Result;
        llvm::Value *Callee;
    };

    using BinaryOperator = std::function<ValuePtr(
        Builder &builder,
        const SourceLocation &where,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs)>;

    struct UnaryResult
    {
        ValuePtr Value;
        bool Assign;
    };

    using UnaryOperator = std::function<UnaryResult(
        Builder &builder,
        const SourceLocation &where,
        const ValuePtr &value)>;

    class Builder
    {
    public:
        Builder(TypeContext &ctx, llvm::LLVMContext &context, const std::string_view &module_id, bool is_main);

        void Close();

        [[nodiscard]] TypeContext &GetCtx() const;

        std::unique_ptr<llvm::Module> &&MoveModule();

        [[nodiscard]] llvm::LLVMContext &GetContext() const;
        [[nodiscard]] llvm::Module &GetModule() const;
        [[nodiscard]] llvm::IRBuilder<> &GetBuilder() const;

        [[nodiscard]] llvm::Value *CreateAlloca(llvm::Type *type, unsigned size = 0) const;
        ValuePtr CreateAlloca(const SourceLocation &where, const TypePtr &type, unsigned size = 0);
        ValuePtr CreateGlobal(const SourceLocation &where, const std::string &name, const TypePtr &type, bool init);

        ValuePtr CreateMember(const SourceLocation &where, const ValuePtr &object, const std::string &name);

        ValuePtr CreateSubscript(const SourceLocation &where, const ValuePtr &array, const ValuePtr &index);
        ValuePtr CreateSubscript(const SourceLocation &where, const ValuePtr &array, llvm::Value *index);
        ValuePtr CreateSubscript(const SourceLocation &where, const ValuePtr &array, unsigned index);

        ValuePtr CreateCast(const SourceLocation &where, const ValuePtr &value, const TypePtr &type);
        [[nodiscard]] llvm::Value *CreateCast(
            const SourceLocation &where,
            const ValueRef &ref,
            const TypePtr &src_type,
            const TypePtr &dst_type) const;

        void GetFormat(llvm::FunctionCallee &callee) const;

        void Push(const std::string &name = {}, const TypePtr &result_type = {});
        void Pop();

        [[nodiscard]] std::string GetName(bool absolute, const std::string &name) const;

        void DefOp(const std::string &sym, const TypePtr &val, const TypePtr &result, llvm::Value *callee);
        void DefOp(
            const std::string &sym,
            const TypePtr &lhs,
            const TypePtr &rhs,
            const TypePtr &result,
            llvm::Value *callee);
        OperatorRef GetOp(const std::string &sym, const TypePtr &val);
        OperatorRef GetOp(const std::string &sym, const TypePtr &lhs, const TypePtr &rhs);

        ValuePtr &DefVar(const SourceLocation &where, const std::string_view &name);
        ValuePtr &GetVar(const SourceLocation &where, const std::string_view &name);

        TypePtr &ResultType();

    private:
        TypeContext &m_Ctx;
        llvm::LLVMContext &m_LLVMContext;

        std::string m_ModuleID;
        bool m_IsMain;

        std::unique_ptr<llvm::Module> m_LLVMModule;
        std::unique_ptr<llvm::IRBuilder<> > m_LLVMBuilder;

        std::map<std::string, std::map<TypePtr, OperatorRef> > m_UnOps;
        std::map<std::string, std::map<TypePtr, std::map<TypePtr, OperatorRef> > > m_BinOps;

        std::vector<StackFrame> m_Stack;
    };
}
