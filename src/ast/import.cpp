#include <utility>
#include <llvm/IR/DerivedTypes.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>

NJS::ImportStatement::ImportStatement(
    SourceLocation where,
    ImportMapping mapping,
    std::filesystem::path filepath,
    std::vector<StatementPtr> functions,
    std::set<std::string> import_module_ids)
    : Statement(std::move(where)),
      Mapping(std::move(mapping)),
      Filepath(std::move(filepath)),
      Functions(std::move(functions)),
      ImportModuleIDs(std::move(import_module_ids))
{
}

void NJS::ImportStatement::CreateModuleCall(const Builder &builder, const std::string &module_id)
{
    if (module_id == "main")
        return;

    const auto module_main = module_id + ".main";
    const auto function_type = llvm::FunctionType::get(builder.GetBuilder().getVoidTy(), false);
    const auto function_callee = builder.GetModule().getOrInsertFunction(module_main, function_type);
    builder.GetBuilder().CreateCall(function_callee);
}

void NJS::ImportStatement::GenVoidLLVM(Builder &builder) const
{
    for (const auto &module_id: ImportModuleIDs)
        CreateModuleCall(builder, module_id);

    const auto module_id = Filepath.filename().replace_extension().string();
    CreateModuleCall(builder, module_id);

    Mapping.MapFunctions(builder, Where, module_id, Functions);
}

std::ostream &NJS::ImportStatement::Print(std::ostream &stream)
{
    return Mapping.Print(stream << "import ") << " from " << '"' << Filepath.string() << '"';
}
