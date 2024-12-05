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
        Builder(Context&, llvm::LLVMContext&, const std::string&, bool);

        void Close();

        [[nodiscard]] Context& Ctx() const;

        std::unique_ptr<llvm::Module>&& MoveModule();

        [[nodiscard]] llvm::LLVMContext& LLVMContext() const;
        [[nodiscard]] llvm::Module& LLVMModule() const;
        [[nodiscard]] llvm::IRBuilder<>& LLVMBuilder() const;

        ValuePtr CreateAlloca(const TypePtr&);
        llvm::Value* CreateAlloca(llvm::Type*, size_t) const;

        ValuePtr CreateGlobal(const std::string&, const TypePtr&, bool, const ValuePtr&);

        void GetFormat(llvm::FunctionCallee&) const;

        void Push(const std::string& = {});
        void Pop();

        [[nodiscard]] std::string ValueName(const std::string&) const;

        ValuePtr& CreateVar(const std::string&);
        ValuePtr& GetVar(const std::string&);

    private:
        Context& m_Ctx;
        llvm::LLVMContext& m_LLVMContext;

        std::string m_ModuleID;

        std::unique_ptr<llvm::Module> m_LLVMModule;
        std::unique_ptr<llvm::IRBuilder<>> m_LLVMBuilder;

        std::vector<StackFrame> m_Stack;
    };
}
