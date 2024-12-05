#include <llvm/Linker/Linker.h>
#include <NJS/Error.hpp>
#include <NJS/Linker.hpp>

NJS::Linker::Linker(const std::string& module_id)
{
    m_LLVMContext = std::make_unique<llvm::LLVMContext>();
    m_LLVMModule = std::make_unique<llvm::Module>(module_id, *m_LLVMContext);
}

llvm::LLVMContext& NJS::Linker::LLVMContext() const
{
    return *m_LLVMContext;
}

llvm::Module& NJS::Linker::LLVMModule() const
{
    return *m_LLVMModule;
}

void NJS::Linker::Link(std::unique_ptr<llvm::Module>&& module) const
{
    if (llvm::Linker::linkModules(LLVMModule(), std::move(module)))
        Error("failed to link modules");
}

void NJS::Linker::Output(llvm::raw_ostream& output_stream) const
{
    m_LLVMModule->print(output_stream, {});
}