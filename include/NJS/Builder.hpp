#pragma once

#include <map>
#include <memory>
#include <vector>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <NJS/Info.hpp>
#include <NJS/NJS.hpp>

namespace NJS
{
    struct StackFrame
    {
        [[nodiscard]] bool Contains(const std::string_view &) const;
        ValuePtr operator[](const std::string_view &) const;
        ValuePtr &operator[](const std::string_view &);

        [[nodiscard]] std::string GetChildName(const std::string_view &) const;

        std::string Name;
        TypePtr ResultType;
        std::map<std::string, ValuePtr> Values;
    };

    using UnaryOperator = std::function<ValuePtr(
        Builder &builder,
        const SourceLocation &where,
        const ValuePtr &value)>;

    using BinaryOperator = std::function<ValuePtr(
        Builder &builder,
        const SourceLocation &where,
        const TypePtr &type,
        llvm::Value *lhs,
        llvm::Value *rhs)>;

    class Builder
    {
    public:
        Builder(TypeContext &ctx, llvm::LLVMContext &context, const std::string_view &module_id, bool is_main);

        void Close();

        [[nodiscard]] TypeContext &GetTypeContext() const;

        std::unique_ptr<llvm::Module> &&MoveModule();

        [[nodiscard]] llvm::LLVMContext &GetContext() const;
        [[nodiscard]] llvm::Module &GetModule() const;
        [[nodiscard]] llvm::IRBuilder<> &GetBuilder() const;

        [[nodiscard]] llvm::Value *CreateAlloca(llvm::Type *type, unsigned size = 0) const;
        ValuePtr CreateAlloca(const SourceLocation &where, const TypePtr &type, unsigned size = 0);
        ValuePtr CreateGlobal(
            const SourceLocation &where,
            const std::string &name,
            const TypePtr &type,
            bool initialize);

        ValuePtr CreateMember(const SourceLocation &where, const ValuePtr &object, const std::string &name);

        ValuePtr CreateSubscript(const SourceLocation &where, ValuePtr array, const ValuePtr &index);
        ValuePtr CreateSubscript(const SourceLocation &where, const ValuePtr &array, unsigned index);

        ValuePtr CreateCast(const SourceLocation &where, const ValuePtr &value, const TypePtr &type);
        [[nodiscard]] llvm::Value *CreateCast(
            const SourceLocation &where,
            const ValueInfo &ref,
            const TypePtr &src_type,
            const TypePtr &dst_type) const;

        void GetFormat(llvm::FunctionCallee &callee) const;

        void StackPush(const std::string_view &name = {}, const TypePtr &result_type = {});
        void StackPop();

        [[nodiscard]] std::string GetName(bool absolute, const std::string_view &name) const;

        void DefineFunction(const std::string_view &name, const FunctionTypePtr &type, llvm::Function *callee);
        FunctionInfo GetFunction(const std::string_view &name, const FunctionTypePtr &type) const;
        FunctionInfo FindFunction(const std::string_view &name, const std::vector<ValuePtr> &arguments) const;

        void DefineOperator(
            const std::string_view &name,
            bool prefix,
            const TypePtr &value_type,
            const TypePtr &result_type,
            llvm::Value *callee);
        void DefineOperator(
            const std::string_view &name,
            const TypePtr &left_type,
            const TypePtr &right_type,
            const TypePtr &result_type,
            llvm::Value *callee);

        OperatorInfo<1> GetOperator(
            const std::string_view &name,
            bool prefix,
            const TypePtr &value_type) const;
        OperatorInfo<2> GetOperator(
            const std::string_view &name,
            const TypePtr &left_type,
            const TypePtr &right_type) const;

        OperatorInfo<1> FindOperator(
            const std::string_view &name,
            bool prefix,
            const ValuePtr &value) const;
        OperatorInfo<2> FindOperator(
            const std::string_view &name,
            const ValuePtr &left_operand,
            const ValuePtr &right_operand) const;

        ValuePtr &DefineVariable(const SourceLocation &where, const std::string_view &name);
        ValuePtr &GetVariable(const SourceLocation &where, const std::string_view &name);

        TypePtr &CurrentFunctionResultType();

    private:
        TypeContext &m_TypeContext;
        llvm::LLVMContext &m_LLVMContext;

        std::string m_ModuleID;
        bool m_IsMain;

        std::unique_ptr<llvm::Module> m_LLVMModule;
        std::unique_ptr<llvm::IRBuilder<>> m_LLVMBuilder;

        std::map<std::string, std::map<bool, std::map<TypePtr, OperatorInfo<1>>>> m_UnaryOperatorMap;
        std::map<std::string, std::map<TypePtr, std::map<TypePtr, OperatorInfo<2>>>> m_BinaryOperatorMap;

        std::map<std::string, std::vector<FunctionInfo>> m_FunctionMap;

        std::vector<StackFrame> m_Stack;
    };
}
