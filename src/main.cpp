#include <filesystem>
#include <fstream>
#include <iostream>
#include <NJS/ArgParser.hpp>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Context.hpp>
#include <NJS/Error.hpp>
#include <NJS/Linker.hpp>
#include <NJS/Parser.hpp>

enum ARG_ID
{
    ARG_ID_HELP,
    ARG_ID_VERSION,
    ARG_ID_OUTPUT,
    ARG_ID_TYPE,
    ARG_ID_MAIN,
};

static llvm::CodeGenFileType to_type(const std::string& str)
{
    static const std::map<std::string, llvm::CodeGenFileType> map
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
    const NJS::Linker& linker,
    const std::string& module_id,
    const bool is_main,
    std::istream& input_stream,
    const std::filesystem::path& input_path)
{
    NJS::Context context;

    NJS::Builder builder(context, linker.LLVMContext(), module_id, is_main);
    NJS::Parser parser(context, input_stream, input_path.string());
    parser.Parse([&](const NJS::StmtPtr& ptr) { ptr->GenLLVM(builder); });

    builder.Close();
    // builder.LLVMModule().print(llvm::errs(), {});
    linker.Link(builder.MoveModule());
}

int main(const int argc, const char** argv)
{
    NJS::ArgParser args({
        {ARG_ID_HELP, {"--help", "-h"}, true},
        {ARG_ID_VERSION, {"--version", "-v"}, true},
        {ARG_ID_OUTPUT, {"--output", "-o"}, false},
        {ARG_ID_TYPE, {"--type", "-t"}, false},
        {ARG_ID_MAIN, {"--main", "-m"}, false},
    });
    args.Parse(argc, argv);

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

    const auto output_module_id = std::filesystem::path(output_filename).filename().replace_extension().string();
    const NJS::Linker linker(output_filename.empty() ? "module" : output_module_id);

    if (input_filenames.empty())
        parse(linker, "main", true, std::cin, {});

    for (const auto& input_filename : input_filenames)
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
    else linker.Emit(llvm::outs(), output_type);
}
