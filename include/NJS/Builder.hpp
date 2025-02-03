#pragma once

#include <map>
#include <memory>
#include <vector>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <NJS/NJS.hpp>
#include <NJS/Operator.hpp>

namespace NJS
{
    struct StackFrame
    {
        [[nodiscard]] bool contains(const std::string&) const;
        ValuePtr operator[](const std::string&) const;
        ValuePtr& operator[](const std::string&);

        [[nodiscard]] std::string ValueName(const std::string&) const;

        std::string ParentName;
        TypePtr ResultType;
        std::map<std::string, ValuePtr> Values;
    };

    typedef std::pair<llvm::Value*, llvm::Value*> ValRef;
    typedef std::pair<TypePtr, llvm::Value*> OpRef;
    typedef std::function<ValuePtr(Builder&, const SourceLocation&, const TypePtr&, llvm::Value*, llvm::Value*)> BinaryOperator;
    typedef std::function<UnaryResult(Builder&, const SourceLocation&, const ValuePtr&)> UnOp;

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
        ValuePtr CreateAlloca(const SourceLocation&, const TypePtr&, unsigned = 0);
        ValuePtr CreateGlobal(const SourceLocation&, const std::string&, const TypePtr&, bool);

        ValuePtr CreateMember(const SourceLocation&, const ValuePtr&, const std::string&);

        ValuePtr CreateSubscript(const SourceLocation&, const ValuePtr&, const ValuePtr&);
        ValuePtr CreateSubscript(const SourceLocation&, const ValuePtr&, llvm::Value*);
        ValuePtr CreateSubscript(const SourceLocation&, const ValuePtr&, unsigned);

        ValuePtr CreateCast(const SourceLocation&, const ValuePtr&, const TypePtr&);
        [[nodiscard]] llvm::Value* CreateCast(const SourceLocation&, ValRef, const TypePtr&, const TypePtr&) const;

        void GetFormat(llvm::FunctionCallee&) const;

        void Push(const std::string& = {}, const TypePtr& = {});
        void Pop();

        [[nodiscard]] std::string GetName(bool, const std::string&) const;

        void DefOp(const std::string&, const TypePtr&, const TypePtr&, llvm::Value*);
        void DefOp(const std::string&, const TypePtr&, const TypePtr&, const TypePtr&, llvm::Value*);
        OpRef GetOp(const std::string&, const TypePtr&);
        OpRef GetOp(const std::string&, const TypePtr&, const TypePtr&);

        ValuePtr& DefVar(const SourceLocation&, const std::string&);
        ValuePtr& GetVar(const SourceLocation&, const std::string&);

        TypePtr& ResultType();

    private:
        TypeContext& m_Ctx;
        llvm::LLVMContext& m_LLVMContext;

        std::string m_ModuleID;
        bool m_IsMain;

        std::unique_ptr<llvm::Module> m_LLVMModule;
        std::unique_ptr<llvm::IRBuilder<>> m_LLVMBuilder;

        std::map<std::string, std::map<TypePtr, OpRef>> m_UnOps;
        std::map<std::string, std::map<TypePtr, std::map<TypePtr, OpRef>>> m_BinOps;

        std::vector<StackFrame> m_Stack;
    };
}
