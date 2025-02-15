#pragma once

#include <memory>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

namespace NJS
{
    class Linker
    {
    public:
        explicit Linker(const std::string &module_id);

        [[nodiscard]] llvm::LLVMContext &LLVMContext() const;
        [[nodiscard]] llvm::Module &LLVMModule() const;

        void Link(std::unique_ptr<llvm::Module> &&module) const;
        void Emit(llvm::raw_pwrite_stream &output_stream, llvm::CodeGenFileType output_type) const;

    private:
        std::unique_ptr<llvm::LLVMContext> m_LLVMContext;
        std::unique_ptr<llvm::Module> m_LLVMModule;
    };
}
