#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Parameter.hpp>
#include <NJS/Type.hpp>

NJS::DestructureArray::DestructureArray(std::vector<ParameterPtr> elements)
    : Parameter(""),
      Elements(std::move(elements))
{
}

bool NJS::DestructureArray::RequireValue()
{
    return true;
}

void NJS::DestructureArray::CreateVars(
    Builder &builder,
    const SourceLocation &where,
    const bool is_const,
    const ValuePtr &value)
{
    for (unsigned i = 0; i < Elements.size(); ++i)
    {
        const auto element = builder.CreateSubscript(where, value, i);
        Elements[i]->CreateVars(builder, where, is_const, element);
    }
}

std::ostream &NJS::DestructureArray::Print(std::ostream &stream)
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
