#include <filesystem>
#include <fstream>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Context.hpp>
#include <NJS/Parser.hpp>

int main(const int argc, const char* const* argv)
{
    if (argc != 2)
        return 1;

    const std::filesystem::path filepath(argv[1]);

    NJS::Context context;
    NJS::Builder builder(context, filepath.filename().replace_extension().string());

    std::ifstream stream(filepath);
    NJS::Parser parser(context, stream, filepath.string());
    parser.Parse([&](const NJS::StmtPtr& ptr)
    {
        // ptr->Print(std::cerr) << std::endl;
        ptr->GenLLVM(builder);
    });
    stream.close();

    builder.Close();
}
