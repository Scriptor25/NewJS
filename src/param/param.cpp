#include <NJS/Context.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Param.hpp>

NJS::Param::Param(std::string name)
    : Name(std::move(name))
{
}

bool NJS::Param::RequireValue()
{
    return false;
}

void NJS::Param::CreateVars(Context& ctx, const TypePtr& type)
{
    if (type && Type && type != Type) Error("cannot assign value of type {} to value of type {}", type, Type);
    ctx.CreateVar(Name) = Type ? Type : type;
}

NJS::DestructureObject::DestructureObject(std::map<std::string, ParamPtr> elements)
    : Param(""), Elements(std::move(elements))
{
}

bool NJS::DestructureObject::RequireValue()
{
    return true;
}

void NJS::DestructureObject::CreateVars(Context& ctx, const TypePtr& type)
{
    if (type && Type && type != Type) Error("cannot assign value of type {} to value of type {}", type, Type);
    for (const auto& [name, element] : Elements)
        element->CreateVars(ctx, (Type ? Type : type)->Member(name));
}

NJS::DestructureArray::DestructureArray(std::vector<ParamPtr> elements)
    : Param(""), Elements(std::move(elements))
{
}

bool NJS::DestructureArray::RequireValue()
{
    return true;
}

void NJS::DestructureArray::CreateVars(Context& ctx, const TypePtr& type)
{
    if (type && Type && type != Type) Error("cannot assign value of type {} to value of type {}", type, Type);
    for (size_t i = 0; i < Elements.size(); ++i)
        Elements[i]->CreateVars(ctx, (Type ? Type : type)->Element(i));
}
