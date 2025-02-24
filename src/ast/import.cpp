#include <utility>
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

void NJS::ImportStatement::GenVoidLLVM(Builder &builder) const
{
    for (auto &sub_module_id: SubModuleIDs)
        builder.CreateModuleCall(sub_module_id);

    Mapping.MapFunctions(builder, Where, ModuleID, Functions);
}

std::ostream &NJS::ImportStatement::Print(std::ostream &stream)
{
    return Mapping.Print(stream << "import ") << " from " << '"' << Filepath.string() << '"';
}
