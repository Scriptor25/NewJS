#include <utility>
#include <llvm/IR/DerivedTypes.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>

NJS::ImportStatement::ImportStatement(
    SourceLocation where,
    ImportMapping mapping,
    std::filesystem::path filepath,
    std::vector<FunctionStatementPtr> functions,
    std::string module_id,
    std::set<std::string> sub_module_ids)
    : Statement(std::move(where)),
      Mapping(std::move(mapping)),
      Filepath(std::move(filepath)),
      Functions(std::move(functions)),
      ModuleID(std::move(module_id)),
      SubModuleIDs(std::move(sub_module_ids))
{
}

void NJS::ImportStatement::CreateModuleCall(const Builder &builder, const std::string &module_id)
{
    if (module_id == "main")
        return;

    const auto module_main = module_id + ".main";
    const auto function_type = llvm::FunctionType::get(builder.GetBuilder().getVoidTy(), false);
    auto function_callee = builder.GetModule().getFunction(module_main);
    if (!function_callee)
        function_callee = llvm::Function::Create(
            function_type,
            llvm::Function::ExternalLinkage,
            module_main,
            builder.GetModule());
    builder.GetBuilder().CreateCall(function_callee);
}

void NJS::ImportStatement::GenVoidLLVM(Builder &builder) const
{
    for (auto &sub_module_id: SubModuleIDs)
        CreateModuleCall(builder, sub_module_id);

    Mapping.MapFunctions(builder, Where, ModuleID, Functions);
}

std::ostream &NJS::ImportStatement::Print(std::ostream &stream)
{
    return Mapping.Print(stream << "import ") << " from " << '"' << Filepath.string() << '"';
}
