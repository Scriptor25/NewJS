#include <llvm/Passes/PassBuilder.h>
#include <llvm/Transforms/AggressiveInstCombine/AggressiveInstCombine.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar/ADCE.h>
#include <llvm/Transforms/Scalar/DCE.h>
#include <llvm/Transforms/Scalar/DeadStoreElimination.h>
#include <llvm/Transforms/Scalar/EarlyCSE.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Scalar/InstSimplifyPass.h>
#include <llvm/Transforms/Scalar/LICM.h>
#include <llvm/Transforms/Scalar/MemCpyOptimizer.h>
#include <llvm/Transforms/Scalar/Reassociate.h>
#include <llvm/Transforms/Scalar/SCCP.h>
#include <llvm/Transforms/Scalar/SimplifyCFG.h>
#include <llvm/Transforms/Utils/Mem2Reg.h>
#include <newjs/pass_manager.hpp>

NJS::PassManager::PassManager(llvm::LLVMContext &llvm_context)
    : m_LLVMContext(llvm_context)
{
    m_FPM = std::make_unique<llvm::FunctionPassManager>();
    m_LAM = std::make_unique<llvm::LoopAnalysisManager>();
    m_FAM = std::make_unique<llvm::FunctionAnalysisManager>();
    m_CGAM = std::make_unique<llvm::CGSCCAnalysisManager>();
    m_MAM = std::make_unique<llvm::ModuleAnalysisManager>();
    m_PIC = std::make_unique<llvm::PassInstrumentationCallbacks>();
    m_SI = std::make_unique<llvm::StandardInstrumentations>(m_LLVMContext, true);

    m_SI->registerCallbacks(*m_PIC, m_MAM.get());

    llvm::PassBuilder pass_builder;
    pass_builder.registerModuleAnalyses(*m_MAM);
    pass_builder.registerFunctionAnalyses(*m_FAM);
    pass_builder.crossRegisterProxies(*m_LAM, *m_FAM, *m_CGAM, *m_MAM);

    m_FPM->addPass(llvm::PromotePass());
    m_FPM->addPass(llvm::MemCpyOptPass());

    m_FPM->addPass(llvm::InstCombinePass());
    m_FPM->addPass(llvm::ReassociatePass());
    m_FPM->addPass(llvm::InstSimplifyPass());

    m_FPM->addPass(llvm::SimplifyCFGPass());
    m_FPM->addPass(llvm::EarlyCSEPass());
    m_FPM->addPass(llvm::GVNPass());
    m_FPM->addPass(llvm::DCEPass());

    m_FPM->addPass(llvm::DSEPass());
    m_FPM->addPass(llvm::SCCPPass());
    m_FPM->addPass(llvm::AggressiveInstCombinePass());

    m_FPM->addPass(llvm::LoopSimplifyPass());

    m_FPM->addPass(llvm::ADCEPass());
}

llvm::FunctionPassManager &NJS::PassManager::FPM() const
{
    return *m_FPM;
}

llvm::LoopAnalysisManager &NJS::PassManager::LAM() const
{
    return *m_LAM;
}

llvm::FunctionAnalysisManager &NJS::PassManager::FAM() const
{
    return *m_FAM;
}

llvm::CGSCCAnalysisManager &NJS::PassManager::CGAM() const
{
    return *m_CGAM;
}

llvm::ModuleAnalysisManager &NJS::PassManager::MAM() const
{
    return *m_MAM;
}

llvm::PassInstrumentationCallbacks &NJS::PassManager::PIC() const
{
    return *m_PIC;
}

llvm::StandardInstrumentations &NJS::PassManager::SI() const
{
    return *m_SI;
}
