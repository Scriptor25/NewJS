#include <fstream>
#include <iostream>
#include <NJS/AST.hpp>
#include <NJS/Context.hpp>
#include <NJS/Parser.hpp>

int main(const int argc, const char* const* argv)
{
    NJS::Context context;

    const std::string filename(argv[1]);
    std::ifstream stream(filename);
    NJS::Parser parser(context, stream, filename);
    parser.Parse([&](const NJS::StmtPtr& ptr)
    {
        std::cout << ptr << std::endl;
    });
    stream.close();
}
