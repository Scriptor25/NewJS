#include <NJS/SourceLocation.hpp>

NJS::SourceLocation::SourceLocation(std::string_view filename)
    : Filename(std::move(filename)),
      Row(1),
      Col(1)
{
}
