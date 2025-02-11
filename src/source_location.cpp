#include <NJS/SourceLocation.hpp>

NJS::SourceLocation::SourceLocation(const std::string &filename)
    : Filename(filename),
      Row(1),
      Col(1)
{
}
