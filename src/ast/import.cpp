#include <utility>
#include <llvm/IR/DerivedTypes.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>

NJS::ImportStmt::ImportStmt(
    SourceLocation where,
    ImportMapping mapping,
    std::filesystem::path filepath,
    std::vector<FunctionStmtPtr> functions)
    : Stmt(std::move(where)),
      Mapping(std::move(mapping)),
      Filepath(std::move(filepath)),
      Functions(std::move(functions))
{
}

NJS::ValuePtr NJS::ImportStmt::GenLLVM(Builder& builder)
{
    const auto module_id = Filepath.filename().replace_extension().string();
    if (module_id != "main")
    {
        const auto module_main = module_id + ".main";
        const auto type = llvm::FunctionType::get(builder.GetBuilder().getVoidTy(), false);
        const auto callee = builder.GetModule().getOrInsertFunction(module_main, type);
        builder.GetBuilder().CreateCall(callee);
    }

    Mapping.MapFunctions(builder, module_id, Functions);
    return {};
}

std::ostream& NJS::ImportStmt::Print(std::ostream& os)
{
    return Mapping.Print(os << "import ") << " from " << '"' << Filepath.string() << '"';
}
