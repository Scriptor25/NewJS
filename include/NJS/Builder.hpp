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

        [[nodiscard]] llvm::Value* CreateAlloca(llvm::Type*, unsigned = 0) const;
        ValuePtr CreateAlloca(const TypePtr&, unsigned = 0);
        ValuePtr CreateGlobal(const std::string&, const TypePtr&, bool);

        ValuePtr CreateMember(const ValuePtr&, const std::string&);

        ValuePtr CreateSubscript(const ValuePtr&, const ValuePtr&);
        ValuePtr CreateSubscript(const ValuePtr&, llvm::Value*);
        ValuePtr CreateSubscript(const ValuePtr&, unsigned);

        ValuePtr CreateCast(const ValuePtr&, const TypePtr&);
        llvm::Value* CreateCast(llvm::Value*, const TypePtr&, const TypePtr&) const;

        void GetFormat(llvm::FunctionCallee&) const;

        void Push(const std::string& = {});
        void Pop();

        [[nodiscard]] std::string GetName(const std::string&) const;

        void DefOp(const std::string&, const TypePtr&, const TypePtr&, const TypePtr&, llvm::Function*);
        std::pair<TypePtr, llvm::Function*> GetOp(const std::string&, const TypePtr&, const TypePtr&);

        ValuePtr& DefVar(const std::string&);
        ValuePtr& GetVar(const SourceLocation&, const std::string&);

    private:
        TypeContext& m_Ctx;
        llvm::LLVMContext& m_LLVMContext;

        std::string m_ModuleID;
        bool m_IsMain;

        std::unique_ptr<llvm::Module> m_LLVMModule;
        std::unique_ptr<llvm::IRBuilder<>> m_LLVMBuilder;

        std::map<std::string, std::map<TypePtr, std::map<TypePtr, std::pair<TypePtr, llvm::Function*>>>> m_BinOps;

        std::vector<StackFrame> m_Stack;
    };
}
