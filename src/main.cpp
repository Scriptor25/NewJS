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
};

static void parse(const NJS::Linker& linker, std::istream& input_stream, const std::filesystem::path& input_path)
{
    NJS::Context context;

    const auto module_id = input_path.filename().replace_extension().string();
    NJS::Builder builder(context, linker.LLVMContext(), module_id);
    NJS::Parser parser(context, input_stream, input_path.string());
    parser.Parse([&](const NJS::StmtPtr& ptr) { ptr->GenLLVM(builder); });

    builder.Close();
    linker.Link(builder.MoveModule());
}

int main(const int argc, const char** argv)
{
    NJS::ArgParser args({
        {ARG_ID_HELP, {"--help", "-h"}, true},
        {ARG_ID_VERSION, {"--version", "-v"}, true},
        {ARG_ID_OUTPUT, {"--output", "-o"}, false},
        {ARG_ID_TYPE, {"--type", "-t"}, false},
    });
    args.Parse(argc, argv);

    std::vector<std::string> input_filenames;
    args.Values(input_filenames);

    std::string output_filename;
    if (args.HasOption(ARG_ID_OUTPUT))
        output_filename = args.Option(ARG_ID_OUTPUT);

    const auto module_id = std::filesystem::path(output_filename).filename().replace_extension().string();
    const NJS::Linker linker(output_filename.empty() ? "module" : module_id);

    if (input_filenames.empty())
        parse(linker, std::cin, {});

    for (const auto& input_filename : input_filenames)
    {
        const std::filesystem::path input_path(input_filename);

        std::ifstream input_stream(input_path);
        if (!input_stream)
            NJS::Error("failed to open input file '{}'", input_filename);

        parse(linker, input_stream, input_path);
        input_stream.close();
    }

    if (!output_filename.empty())
    {
        std::error_code error_code;
        llvm::raw_fd_ostream output_stream(output_filename, error_code);

        if (error_code)
            NJS::Error("failed to open output file '{}': {}", output_filename, error_code.message());

        linker.Output(output_stream);
        output_stream.close();
    }
    else linker.Output(llvm::outs());
}
