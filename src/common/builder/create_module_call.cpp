#include <newjs/builder.hpp>

void NJS::Builder::CreateModuleCall(const std::string &module_id)
{
    if (module_id == "main" || m_ModuleCalls.contains(module_id))
        return;

    m_ModuleCalls.insert(module_id);

    const auto type = llvm::FunctionType::get(GetBuilder().getVoidTy(), false);
    const auto name = module_id + ".main";
    const auto callee = GetOrCreateFunction(type, llvm::Function::ExternalLinkage, name);
    GetBuilder().CreateCall(callee);
}
