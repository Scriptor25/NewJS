#include <filesystem>
#include <fstream>
#include <iostream>
#include <newjs/arg_parser.hpp>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/error.hpp>
#include <newjs/linker.hpp>
#include <newjs/parser.hpp>
#include <newjs/type_context.hpp>

using namespace std::string_view_literals;

enum ARG_ID
{
    ARG_ID_HELP,
    ARG_ID_MAIN,
    ARG_ID_OUTPUT,
    ARG_ID_PRINT,
    ARG_ID_TARGET,
    ARG_ID_TYPE,
    ARG_ID_VERSION,
};

static llvm::CodeGenFileType to_type(const std::string_view &str)
{
    static const std::map<std::string_view, llvm::CodeGenFileType> map
    {
        {"llvm"sv, llvm::CodeGenFileType::Null},
        {"obj"sv, llvm::CodeGenFileType::ObjectFile},
        {"asm"sv, llvm::CodeGenFileType::AssemblyFile},
    };
    return map.contains(str) ? map.at(str) : llvm::CodeGenFileType::Null;
}

static void parse(
    const bool print,
    const NJS::Linker &linker,
    const std::string &module_id,
    const bool is_main,
    std::istream &input_stream,
    const std::filesystem::path &input_path)
{
    const auto input_filename = input_path.string();

    NJS::TypeContext type_context;
    NJS::Builder builder(type_context, linker.LLVMContext(), module_id, input_filename, is_main);

    std::map<std::string, NJS::Macro> macro_map;
    NJS::Parser parser(type_context, builder, input_stream, NJS::SourceLocation(input_filename), macro_map, is_main);

    parser.Parse(
        [&](const NJS::StatementPtr &statement)
        {
            if (print)
            {
                statement->Print(std::cerr) << std::endl;
            }

            statement->GenIntermediate(builder, false);
        });

    builder.Close();
    linker.Link(builder.MoveModule());
}

int main(const int argc, const char **argv) try
{
    NJS::ArgParser args(
        {
            {ARG_ID_HELP, "Display this help text.", {"--help", "-h"}, true},
            {ARG_ID_VERSION, "Display the program version.", {"--version", "-v"}, true},
            {ARG_ID_OUTPUT, "Specify the output filename.", {"--output", "-o"}, false},
            {
                ARG_ID_TYPE,
                "Specify the output file type. Available values are 'llvm' for the intermediate representation, 'obj' for a compiled object file and 'asm' for an assembler file.",
                {"--type", "-t"},
                false
            },
            {ARG_ID_MAIN, "Specify which module name is the main module.", {"--main", "-m"}, false},
            {ARG_ID_TARGET, "Specify output target triple, defaults to host target.", {"--triple", "-T"}, false},
            {ARG_ID_PRINT, "Print out the program AST", {"--print", "-p"}, true},
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
    args.Option(ARG_ID_MAIN, module_main, "main");

    llvm::CodeGenFileType output_type;
    {
        std::string type_str;
        args.Option(ARG_ID_TYPE, type_str, "llvm");
        output_type = to_type(type_str);
    }

    const auto print = args.Flag(ARG_ID_PRINT);

    const auto output_module_id = std::filesystem::path(output_filename).filename().replace_extension().string();
    const NJS::Linker linker(output_module_id, output_filename);

    if (input_filenames.empty())
    {
        parse(print, linker, "main", true, std::cin, {});
    }

    for (const auto &input_filename: input_filenames)
    {
        if (!std::filesystem::exists(input_filename))
        {
            NJS::Error("failed to open input file '{}': file does not exist", input_filename);
        }

        const auto input_path = std::filesystem::canonical(input_filename);
        const auto module_id = input_path.filename().replace_extension().string();

        std::ifstream input_stream(input_path);
        if (!input_stream)
        {
            NJS::Error("failed to open input file '{}'", input_filename);
        }

        parse(print, linker, module_id, module_id == module_main, input_stream, input_path);
        input_stream.close();
    }

    std::string target_triple;
    args.Option(ARG_ID_TARGET, target_triple);

    if (!output_filename.empty())
    {
        std::error_code error_code;
        llvm::raw_fd_ostream output_stream(output_filename, error_code);

        if (error_code)
        {
            NJS::Error("failed to open output file '{}': {}", output_filename, error_code.message());
        }

        linker.Emit(output_stream, output_type, target_triple);
        output_stream.close();
    }
    else
    {
        linker.Emit(llvm::outs(), output_type, target_triple);
    }
}
catch (const NJS::RTError &error)
{
    error.Print(std::cerr);
    return 1;
}
