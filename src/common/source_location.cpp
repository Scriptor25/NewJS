#include <newjs/location.hpp>

NJS::SourceLocation::SourceLocation(std::string filename)
    : Filename(std::move(filename)),
      Row(1)
{
}

NJS::SourceLocation::SourceLocation(std::string filename, const unsigned row, const unsigned column)
    : Filename(std::move(filename)),
      Row(row),
      Column(column)
{
}
