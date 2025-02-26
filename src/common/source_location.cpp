#include <newjs/location.hpp>

NJS::SourceLocation::SourceLocation(std::string filename)
    : Filename(std::move(filename)),
      Row(1),
      Col(1)
{
}
