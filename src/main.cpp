#include <filesystem>
#include <fstream>
#include <iostream>
#include <NJS/ArgParser.hpp>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Linker.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TemplateContext.hpp>
#include <NJS/TypeContext.hpp>

using namespace std::string_view_literals;

enum ARG_ID
{
    ARG_ID_MAIN,
    ARG_ID_OUTPUT,
    ARG_ID_TYPE,
    ARG_ID_HELP,
    ARG_ID_VERSION,
};

static llvm::CodeGenFileType to_type(const std::string_view &str)
{
    static const std::map<std::string_view, llvm::CodeGenFileType> map
    {
        {"llvm", llvm::CodeGenFileType::Null},
        {"obj", llvm::CodeGenFileType::ObjectFile},
        {"asm", llvm::CodeGenFileType::AssemblyFile},
    };
    if (map.contains(str))
        return map.at(str);
    return {};
}

static void parse(
    const NJS::Linker &linker,
    const std::string_view &module_id,
    const bool is_main,
    std::istream &input_stream,
    const std::filesystem::path &input_path)
{
    NJS::TypeContext type_ctx;
    NJS::Builder builder(type_ctx, linker.LLVMContext(), module_id, is_main);

    NJS::TemplateContext template_ctx(builder);
    std::map<std::string, NJS::Macro> macros;
    NJS::Parser parser(type_ctx, template_ctx, input_stream, NJS::SourceLocation(input_path.string()), macros);

    parser.Parse(
        [&](const NJS::StmtPtr &ptr)
        {
            ptr->GenVoidLLVM(builder);
        });

    builder.Close();
    linker.Link(builder.MoveModule());
}

int main(const int argc, const char **argv)
{
    NJS::ArgParser args(
        {
            {ARG_ID_HELP, "Display this help text."sv, {"--help"sv, "-h"sv}, true},
            {ARG_ID_VERSION, "Display the program version."sv, {"--version"sv, "-v"sv}, true},
            {ARG_ID_OUTPUT, "Specify the output filename."sv, {"--output"sv, "-o"sv}, false},
            {
                ARG_ID_TYPE,
                "Specify the output file type. Available values are 'llvm' for the intermediate representation, 'obj' for a compiled object file and 'asm' for an assembler file."sv,
                {"--type"sv, "-t"sv},
                false
            },
            {ARG_ID_MAIN, "Specify which module name is the main module."sv, {"--main"sv, "-m"sv}, false},
        });
    args.Parse(argc, argv);

    if (args.Flag(ARG_ID_VERSION))
    {
        std::cerr << "NewJS [v1.0.0]" << std::endl;
        if (argc == 2)
            return 0;
    }

    if (args.IsEmpty() || args.Flag(ARG_ID_HELP))
    {
        args.Print();
        return 0;
    }

    std::vector<std::string> input_filenames;
    args.Values(input_filenames);

    std::string output_filename;
    args.Option(ARG_ID_OUTPUT, output_filename);

    std::string module_main;
    args.Option(ARG_ID_MAIN, module_main, "main"sv);

    llvm::CodeGenFileType output_type;
    {
        std::string type_str;
        args.Option(ARG_ID_TYPE, type_str, "llvm"sv);
        output_type = to_type(type_str);
    }

    const auto output_module_id = std::filesystem::path(output_filename).filename().replace_extension().string();
    const NJS::Linker linker(output_module_id.empty() ? "module"sv : output_module_id);

    if (input_filenames.empty())
        parse(linker, "main"sv, true, std::cin, {});

    for (const auto &input_filename: input_filenames)
    {
        const std::filesystem::path input_path(input_filename);
        const auto module_id = input_path.filename().replace_extension().string();

        std::ifstream input_stream(input_path);
        if (!input_stream)
            NJS::Error("failed to open input file '{}'", input_filename);

        parse(linker, module_id, module_id == module_main, input_stream, input_path);
        input_stream.close();
    }

    if (!output_filename.empty())
    {
        std::error_code error_code;
        llvm::raw_fd_ostream output_stream(output_filename, error_code);

        if (error_code)
            NJS::Error("failed to open output file '{}': {}", output_filename, error_code.message());

        linker.Emit(output_stream, output_type);
        output_stream.close();
    }
    else
        linker.Emit(llvm::outs(), output_type);
}
