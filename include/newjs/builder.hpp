#pragma once

#include <map>
#include <memory>
#include <vector>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/Passes/StandardInstrumentations.h>
#include <newjs/info.hpp>
#include <newjs/newjs.hpp>
#include <newjs/pass_manager.hpp>

namespace NJS
{
    struct StackFrame
    {
        [[nodiscard]] bool Contains(const std::string &) const;
        const std::pair<bool, ValuePtr> &operator[](const std::string &) const;
        std::pair<bool, ValuePtr> &operator[](const std::string &);

        [[nodiscard]] std::string GetChildName(const std::string &) const;

        bool IsFunctionEntry;
        std::string Name;
        ReferenceInfo Result;
        llvm::BasicBlock *HeadBlock;
        llvm::BasicBlock *TailBlock;
        std::map<std::string, std::pair<bool, ValuePtr>> Values;
    };

    struct MemberValue
    {
        ValuePtr Value;
        bool IsReference;
    };

    class Builder
    {
    public:
        Builder(
            TypeContext &type_context,
            llvm::LLVMContext &llvm_context,
            const std::string &module_id,
            const std::string &source_filename,
            bool is_main);

        void Close();

        [[nodiscard]] const std::string &GetModuleID() const;

        [[nodiscard]] TypeContext &GetTypeContext() const;

        std::unique_ptr<llvm::Module> &&MoveModule();

        [[nodiscard]] llvm::LLVMContext &GetContext() const;
        [[nodiscard]] llvm::Module &GetModule() const;
        [[nodiscard]] llvm::IRBuilder<> &GetBuilder() const;

        void Optimize(llvm::Function *function) const;

        [[nodiscard]] llvm::Value *CreateAlloca(llvm::Type *type, unsigned count = 0) const;
        [[nodiscard]] ValuePtr CreateAlloca(const TypePtr &type, bool is_const, unsigned count = 0) const;
        ValuePtr CreateGlobal(
            const std::string &name,
            const TypePtr &type,
            bool is_const,
            bool initialize,
            llvm::Constant *initializer = {}) const;

        MemberValue CreateMember(const ValuePtr &value, const std::string &name);
        MemberValue CreateMember(const ValuePtr &value, unsigned index);

        [[nodiscard]] ValuePtr CreateSubscript(ValuePtr array, const ValuePtr &index) const;
        [[nodiscard]] ValuePtr CreateSubscript(const ValuePtr &array, unsigned index) const;

        [[nodiscard]] ValuePtr CreateCast(const ValuePtr &value, const TypePtr &type) const;
        [[nodiscard]] llvm::Value *CreateCast(
            const ValueInfo &ref,
            const TypePtr &src_type,
            const TypePtr &dst_type) const;

        void CreateModuleCall(const std::string &module_id);

        llvm::Function *GetOrCreateFunction(
            llvm::FunctionType *type,
            llvm::GlobalValue::LinkageTypes linkage,
            const std::string &name) const;

        void GetFormat(llvm::FunctionCallee &callee) const;

        void StackPush(
            const std::string &name = {},
            const ReferenceInfo &result = {},
            llvm::BasicBlock *head_block = nullptr,
            llvm::
            BasicBlock *tail_block = nullptr);
        void StackPop();

        [[nodiscard]] std::string GetName(bool absolute, const std::string &name) const;

        void DefineOperator(
            const ReferenceInfo &callee_info,
            const std::vector<ReferenceInfo> &parameter_infos,
            bool is_var_arg,
            const ReferenceInfo &result_info,
            llvm::Value *callee);
        void DefineOperator(
            const std::string &name,
            bool prefix,
            const ReferenceInfo &value,
            const ReferenceInfo &result,
            llvm::Value *callee);
        void DefineOperator(
            const std::string &name,
            const ReferenceInfo &left,
            const ReferenceInfo &right,
            const ReferenceInfo &result,
            llvm::Value *callee);

        [[nodiscard]] CallOperatorInfo GetOperator(const ReferenceInfo &callee_info);
        [[nodiscard]] OperatorInfo<1> GetOperator(
            const std::string &name,
            bool prefix,
            const ReferenceInfo &value) const;
        [[nodiscard]] OperatorInfo<2> GetOperator(
            const std::string &name,
            const ReferenceInfo &left,
            const ReferenceInfo &right) const;

        [[nodiscard]] CallOperatorInfo FindOperator(const ValuePtr &callee);
        [[nodiscard]] OperatorInfo<1> FindOperator(
            const std::string &name,
            bool prefix,
            const ValuePtr &value) const;
        [[nodiscard]] OperatorInfo<2> FindOperator(
            const std::string &name,
            const ValuePtr &left,
            const ValuePtr &right) const;

        ValuePtr &DefineVariable(const std::string &name, bool is_global);
        [[nodiscard]] const ValuePtr &GetVariable(const std::string &name) const;
        ValuePtr &GetOrDefineVariable(const std::string &name, bool is_global);

        ReferenceInfo &CurrentFunctionResult();
        [[nodiscard]] llvm::BasicBlock *CurrentHeadBlock() const;
        [[nodiscard]] llvm::BasicBlock *CurrentTailBlock() const;

        llvm::Constant *GetString(const std::string &value);

        void PushLastObjectContext();
        void PopLastObjectContext();
        void SetLastObject(const ValuePtr &object);
        ValuePtr GetLastObject() const;

        static std::string GetBinaryOperatorName(const std::string &name);
        static std::string GetUnaryOperatorName(const std::string &name);
        static std::string GetFunctionName(
            const std::string &module_id,
            const std::string &name,
            const std::vector<ParameterPtr> &parameters,
            bool is_var_arg,
            bool is_extern,
            bool is_operator);

    private:
        std::string m_ModuleID;
        bool m_IsMain;

        std::set<std::string> m_ModuleCalls;

        TypeContext &m_TypeContext;
        llvm::LLVMContext &m_LLVMContext;

        std::unique_ptr<llvm::Module> m_LLVMModule;
        std::unique_ptr<llvm::IRBuilder<>> m_LLVMBuilder;

        PassManager m_PassManager;

        llvm::Function *m_Function;

        ReferenceInfoMap<CallOperatorInfo> m_CallOperatorMap;
        std::map<std::string, std::map<bool, ReferenceInfoMap<OperatorInfo<1>>>> m_UnaryOperatorMap;
        std::map<std::string, ReferenceInfoMap<ReferenceInfoMap<OperatorInfo<2>>>> m_BinaryOperatorMap;

        std::vector<StackFrame> m_Stack;

        std::map<std::string, llvm::Constant *> m_StringTable;

        std::vector<ValuePtr> m_LastObject;
    };
}
