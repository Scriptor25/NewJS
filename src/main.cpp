#include <fstream>
#include <iostream>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Context.hpp>
#include <NJS/Parser.hpp>

int main(const int argc, const char* const* argv)
{
    if (argc != 2)
        return 1;

    NJS::Context context;
    NJS::Builder builder(context, "module");

    const std::string filename(argv[1]);
    std::ifstream stream(filename);
    NJS::Parser parser(context, stream, filename);
    parser.Parse([&](const NJS::StmtPtr& ptr)
    {
        // ptr->Print(std::cerr) << std::endl;
        ptr->GenLLVM(builder);
    });
    stream.close();

    builder.Close();
}
