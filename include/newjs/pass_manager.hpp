#pragma once

#include <memory>
#include <llvm/Analysis/CGSCCPassManager.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/StandardInstrumentations.h>

namespace NJS
{
    class PassManager
    {
    public:
        explicit PassManager(llvm::LLVMContext &llvm_context);

        llvm::FunctionPassManager &FPM() const;
        llvm::LoopAnalysisManager &LAM() const;
        llvm::FunctionAnalysisManager &FAM() const;
        llvm::CGSCCAnalysisManager &CGAM() const;
        llvm::ModuleAnalysisManager &MAM() const;
        llvm::PassInstrumentationCallbacks &PIC() const;
        llvm::StandardInstrumentations &SI() const;

    private:
        llvm::LLVMContext &m_LLVMContext;

        std::unique_ptr<llvm::FunctionPassManager> m_FPM;
        std::unique_ptr<llvm::LoopAnalysisManager> m_LAM;
        std::unique_ptr<llvm::FunctionAnalysisManager> m_FAM;
        std::unique_ptr<llvm::CGSCCAnalysisManager> m_CGAM;
        std::unique_ptr<llvm::ModuleAnalysisManager> m_MAM;
        std::unique_ptr<llvm::PassInstrumentationCallbacks> m_PIC;
        std::unique_ptr<llvm::StandardInstrumentations> m_SI;
    };
}
