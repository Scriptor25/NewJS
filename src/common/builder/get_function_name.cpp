#include <newjs/builder.hpp>
#include <newjs/parameter.hpp>

std::string NJS::Builder::GetFunctionName(
    const std::string &module_id,
    const std::string &name,
    const std::vector<ParameterPtr> &parameters,
    const bool is_var_arg,
    const bool is_extern,
    const bool is_operator)
{
    if (is_extern)
        return name;

    auto prefix = module_id.empty() ? "" : (module_id + '.');

    if (is_operator)
    {
        if (parameters.size() == 1)
            return prefix
                   + (is_var_arg ? std::string() : name)
                   + std::to_string(parameters[0]->Info.GetHash())
                   + (is_var_arg ? name : std::string());
        if (parameters.size() == 2)
            return prefix
                   + std::to_string(parameters[0]->Info.GetHash())
                   + name
                   + std::to_string(parameters[1]->Info.GetHash());
        Error("TODO");
    }

    return prefix + name;
}
