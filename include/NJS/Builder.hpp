#pragma once

#include <map>
#include <memory>
#include <vector>
#include <llvm/Analysis/CGSCCPassManager.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/StandardInstrumentations.h>
#include <NJS/Info.hpp>
#include <NJS/NJS.hpp>
#include <NJS/SourceLocation.hpp>

namespace NJS
{
    struct StackFrame
    {
        [[nodiscard]] bool Contains(const std::string &) const;
        ValuePtr operator[](const std::string &) const;
        ValuePtr &operator[](const std::string &);

        [[nodiscard]] std::string GetChildName(const std::string &) const;

        std::string Name;
        TypePtr ResultType;
        std::map<std::string, ValuePtr> Values;
    };

    struct Passes
    {
        std::unique_ptr<llvm::FunctionPassManager> FPM;
        std::unique_ptr<llvm::LoopAnalysisManager> LAM;
        std::unique_ptr<llvm::FunctionAnalysisManager> FAM;
        std::unique_ptr<llvm::CGSCCAnalysisManager> CGAM;
        std::unique_ptr<llvm::ModuleAnalysisManager> MAM;
        std::unique_ptr<llvm::PassInstrumentationCallbacks> PIC;
        std::unique_ptr<llvm::StandardInstrumentations> SI;
    };

    class Builder
    {
    public:
        Builder(
            TypeContext &ctx,
            llvm::LLVMContext &context,
            const std::string &module_id,
            const std::string &source_filename,
            bool is_main);

        void Close();

        const std::string &GetModuleID() const;

        [[nodiscard]] TypeContext &GetTypeContext() const;

        std::unique_ptr<llvm::Module> &&MoveModule();

        [[nodiscard]] llvm::LLVMContext &GetContext() const;
        [[nodiscard]] llvm::Module &GetModule() const;
        [[nodiscard]] llvm::IRBuilder<> &GetBuilder() const;

        void Optimize(llvm::Function *function) const;

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

        void StackPush(const std::string &name = {}, const TypePtr &result_type = {});
        void StackPop();

        [[nodiscard]] std::string GetName(bool absolute, const std::string &name) const;

        void DefineOperator(
            const std::string &name,
            bool prefix,
            const TypePtr &value_type,
            const TypePtr &result_type,
            llvm::Value *callee);
        void DefineOperator(
            const std::string &name,
            const TypePtr &left_type,
            const TypePtr &right_type,
            const TypePtr &result_type,
            llvm::Value *callee);

        [[nodiscard]] OperatorInfo<1> GetOperator(
            const std::string &name,
            bool prefix,
            const TypePtr &value_type) const;
        [[nodiscard]] OperatorInfo<2> GetOperator(
            const std::string &name,
            const TypePtr &left_type,
            const TypePtr &right_type) const;

        [[nodiscard]] OperatorInfo<1> FindOperator(
            const std::string &name,
            bool prefix,
            const ValuePtr &value) const;
        [[nodiscard]] OperatorInfo<2> FindOperator(
            const std::string &name,
            const ValuePtr &left_operand,
            const ValuePtr &right_operand) const;

        ValuePtr &DefineVariable(const SourceLocation &where, const std::string &name);
        ValuePtr &GetVariable(const SourceLocation &where, const std::string &name);
        ValuePtr &GetOrDefineVariable(const std::string &name);

        TypePtr &CurrentFunctionResultType();

    private:
        std::string m_ModuleID;
        bool m_IsMain;

        TypeContext &m_TypeContext;
        llvm::LLVMContext &m_LLVMContext;

        std::unique_ptr<llvm::Module> m_LLVMModule;
        std::unique_ptr<llvm::IRBuilder<>> m_LLVMBuilder;

        Passes m_Passes;

        std::map<std::string, std::map<bool, std::map<TypePtr, OperatorInfo<1>>>> m_UnaryOperatorMap;
        std::map<std::string, std::map<TypePtr, std::map<TypePtr, OperatorInfo<2>>>> m_BinaryOperatorMap;

        std::vector<StackFrame> m_Stack;
    };
}
