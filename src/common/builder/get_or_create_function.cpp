#include <newjs/builder.hpp>

llvm::Function *NJS::Builder::GetOrCreateFunction(
    llvm::FunctionType *type,
    const llvm::GlobalValue::LinkageTypes linkage,
    const std::string &name) const
{
    auto function = GetModule().getFunction(name);
    if (!function)
        function = llvm::Function::Create(
            type,
            linkage,
            name,
            GetModule());
    return function;
}
