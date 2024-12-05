#include <llvm/IR/DerivedTypes.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>

NJS::ImportStmt::ImportStmt(
    ImportMapping mapping,
    std::filesystem::path filepath,
    std::vector<FunctionStmtPtr> functions)
    : Mapping(std::move(mapping)), Filepath(std::move(filepath)), Functions(std::move(functions))
{
}

NJS::ValuePtr NJS::ImportStmt::GenLLVM(Builder& builder)
{
    const auto module_id = Filepath.filename().replace_extension().string();
    if (module_id != "main")
    {
        const auto module_main = module_id + ".main";
        const auto type = llvm::FunctionType::get(builder.LLVMBuilder().getVoidTy(), false);
        const auto callee = builder.LLVMModule().getOrInsertFunction(module_main, type);
        builder.LLVMBuilder().CreateCall(callee);
    }

    Mapping.MapFunctions(builder, module_id, Functions);
    return {};
}

std::ostream& NJS::ImportStmt::Print(std::ostream& os)
{
    return Mapping.Print(os << "import ") << " from " << '"' << Filepath.string() << '"';
}
