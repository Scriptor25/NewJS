#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>

NJS::ImportStatement::ImportStatement(
    SourceLocation where,
    ImportMapping mapping,
    std::filesystem::path filepath,
    std::vector<ExportStatementPtr> exports,
    std::string module_id,
    std::set<std::string> sub_module_ids)
    : Statement(std::move(where)),
      Mapping(std::move(mapping)),
      Filepath(std::move(filepath)),
      Exports(std::move(exports)),
      ModuleID(std::move(module_id)),
      SubModuleIDs(std::move(sub_module_ids))
{
}

std::ostream &NJS::ImportStatement::Print(std::ostream &stream) const
{
    return Mapping.Print(stream << "import ") << " from " << '"' << Filepath.string() << '"';
}

void NJS::ImportStatement::_GenIntermediate(Builder &builder, bool)
{
    for (auto &sub_module_id: SubModuleIDs)
        builder.CreateModuleCall(sub_module_id);

    Mapping.MapValues(builder, ModuleID, Exports);
}
