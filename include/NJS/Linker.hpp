#pragma once

#include <memory>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

namespace NJS
{
    class Linker
    {
    public:
        explicit Linker(const std::string&);

        llvm::LLVMContext& LLVMContext() const;
        llvm::Module& LLVMModule() const;

        void Link(std::unique_ptr<llvm::Module>&&) const;
        void Emit(llvm::raw_ostream&, llvm::CodeGenFileType) const;

    private:
        std::unique_ptr<llvm::LLVMContext> m_LLVMContext;
        std::unique_ptr<llvm::Module> m_LLVMModule;
    };
}
