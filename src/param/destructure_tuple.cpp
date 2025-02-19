#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Parameter.hpp>
#include <NJS/Type.hpp>

NJS::DestructureTuple::DestructureTuple(SourceLocation where, std::vector<ParameterPtr> elements)
    : Parameter(std::move(where), {}),
      Elements(std::move(elements))
{
}

bool NJS::DestructureTuple::RequireValue()
{
    return true;
}

void NJS::DestructureTuple::CreateVars(
    Builder &builder,
    const ValuePtr value,
    const unsigned flags)
{
    for (unsigned i = 0; i < Elements.size(); ++i)
    {
        const auto element = builder.CreateSubscript(Where, value, i);
        Elements[i]->CreateVars(builder, element, flags);
    }
}

std::ostream &NJS::DestructureTuple::Print(std::ostream &stream)
{
    stream << "[ ";
    for (unsigned i = 0; i < Elements.size(); ++i)
    {
        if (i > 0)
            stream << ", ";
        Elements[i]->Print(stream);
    }
    stream << " ]";
    if (Type)
        Type->Print(stream << ": ");
    return stream;
}
