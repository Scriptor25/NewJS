#include <newjs/parser.hpp>

NJS::StatementPtr NJS::Parser::ParseExportStatement()
{
    auto where = Expect("export").Where;
    auto value = ParseStatement();
    return std::make_shared<ExportStatement>(where, value);
}
