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
        [[nodiscard]] bool contains(const std::string&) const;
        ValuePtr operator[](const std::string&) const;
        ValuePtr& operator[](const std::string&);

        [[nodiscard]] std::string ValueName(const std::string&) const;

        std::string ParentName;
        std::map<std::string, ValuePtr> Values;
    };

    class Builder
    {
    public:
        Builder(TypeContext&, llvm::LLVMContext&, const std::string&, bool);

        void Close();

        [[nodiscard]] TypeContext& GetCtx() const;

        std::unique_ptr<llvm::Module>&& MoveModule();

        [[nodiscard]] llvm::LLVMContext& GetContext() const;
        [[nodiscard]] llvm::Module& GetModule() const;
        [[nodiscard]] llvm::IRBuilder<>& GetBuilder() const;

        [[nodiscard]] llvm::Value* CreateMalloc(size_t) const;
        [[nodiscard]] llvm::Value* CreateMalloc(llvm::Value*) const;
        [[nodiscard]] llvm::Value* CreateRealloc(llvm::Value*, llvm::Value*) const;
        [[nodiscard]] llvm::Value* CreateMemcpy(llvm::Value*, llvm::Value*, llvm::Value*) const;
        [[nodiscard]] llvm::Value* CreateStrlen(llvm::Value*) const;
        [[nodiscard]] llvm::Value* CreateAlloca(llvm::Type*, size_t = 0) const;
        ValuePtr CreateAlloca(const TypePtr&, size_t = 0);
        ValuePtr CreateGlobal(const std::string&, const TypePtr&, bool);

        llvm::Value* CreateEmpty(const TypePtr&);

        ValuePtr CreateMember(const ValuePtr&, const std::string&);
        ValuePtr CreateSubscript(const ValuePtr&, size_t);
        ValuePtr CreateSubscript(const ValuePtr&, llvm::Value*);

        ValuePtr CreateVector(const TypePtr&, llvm::Value*, llvm::Value*);
        bool CreateVectorStoreArray(const ValuePtr&, llvm::Value*);
        ValuePtr CreateVectorPush(const ValuePtr&, const ValuePtr&);
        ValuePtr CreateVectorPop(const ValuePtr&, const ValuePtr&);
        [[nodiscard]] llvm::Value* CreatePtrToVectorPtr(llvm::Value*) const;
        [[nodiscard]] llvm::Value* CreatePtrToVectorSize(llvm::Value*) const;

        void GetFormat(llvm::FunctionCallee&) const;

        void Push(const std::string& = {});
        void Pop();

        [[nodiscard]] std::string GetName(const std::string&) const;

        ValuePtr& DefVar(const std::string&);
        ValuePtr& GetVar(const std::string&);

    private:
        TypeContext& m_Ctx;
        llvm::LLVMContext& m_LLVMContext;

        std::string m_ModuleID;

        std::unique_ptr<llvm::Module> m_LLVMModule;
        std::unique_ptr<llvm::IRBuilder<>> m_LLVMBuilder;

        std::vector<StackFrame> m_Stack;
    };
}
