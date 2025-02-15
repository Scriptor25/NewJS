#include <utility>
#include <llvm/IR/DerivedTypes.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>

NJS::ImportStatement::ImportStatement(
    SourceLocation where,
    ImportMapping mapping,
    std::filesystem::path filepath,
    std::vector<StatementPtr> functions)
    : Statement(std::move(where)),
      Mapping(std::move(mapping)),
      Filepath(std::move(filepath)),
      Functions(std::move(functions))
{
}

void NJS::ImportStatement::GenVoidLLVM(Builder &builder) const
{
    const auto module_id = Filepath.filename().replace_extension().string();
    if (module_id != "main")
    {
        const auto module_main = module_id + ".main";
        const auto function_type = llvm::FunctionType::get(builder.GetBuilder().getVoidTy(), false);
        const auto function_callee = builder.GetModule().getOrInsertFunction(module_main, function_type);
        builder.GetBuilder().CreateCall(function_callee);
    }

    Mapping.MapFunctions(builder, Where, module_id, Functions);
}

std::ostream &NJS::ImportStatement::Print(std::ostream &stream)
{
    return Mapping.Print(stream << "import ") << " from " << '"' << Filepath.string() << '"';
}
