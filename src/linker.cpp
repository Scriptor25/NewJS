#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Linker/Linker.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>
#include <NJS/Error.hpp>
#include <NJS/Linker.hpp>

NJS::Linker::Linker(const std::string_view &module_id)
{
    m_LLVMContext = std::make_unique<llvm::LLVMContext>();
    m_LLVMModule = std::make_unique<llvm::Module>(module_id, *m_LLVMContext);
}

llvm::LLVMContext &NJS::Linker::LLVMContext() const
{
    return *m_LLVMContext;
}

llvm::Module &NJS::Linker::LLVMModule() const
{
    return *m_LLVMModule;
}

void NJS::Linker::Link(std::unique_ptr<llvm::Module> &&module) const
{
    if (llvm::Linker::linkModules(LLVMModule(), std::move(module)))
        Error("failed to link modules");
}

void NJS::Linker::Emit(llvm::raw_pwrite_stream &output_stream, const llvm::CodeGenFileType output_type) const
{
    if (output_type == llvm::CodeGenFileType::Null)
    {
        m_LLVMModule->print(output_stream, {});
        return;
    }

    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    std::string error;

    const auto target_triple = llvm::sys::getDefaultTargetTriple();
    const auto target = llvm::TargetRegistry::lookupTarget(target_triple, error);

    if (!target)
        Error("failed to lookup target for triple '{}': {}", target_triple, error);

    constexpr auto CPU = "generic";
    constexpr auto FEATURES = "";

    const llvm::TargetOptions opt;

    const auto target_machine = target->createTargetMachine(target_triple, CPU, FEATURES, opt, llvm::Reloc::PIC_);

    m_LLVMModule->setDataLayout(target_machine->createDataLayout());
    m_LLVMModule->setTargetTriple(target_triple);

    llvm::legacy::PassManager pass_manager;
    target_machine->addPassesToEmitFile(pass_manager, output_stream, nullptr, output_type);
    pass_manager.run(*m_LLVMModule);
    output_stream.flush();
}
