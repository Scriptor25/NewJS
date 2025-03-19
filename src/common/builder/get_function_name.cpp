#include <newjs/builder.hpp>
#include <newjs/parameter.hpp>

using namespace std::string_view_literals;

std::string NJS::Builder::GetBinaryOperatorName(const std::string &name)
{
    static const std::map<std::string_view, std::string_view> operator_map
    {
        {"[]"sv, "subscript"sv},
        {"=="sv, "eq"sv},
        {"!="sv, "ne"sv},
        {"<"sv, "lt"sv},
        {"<="sv, "le"sv},
        {">"sv, "gt"sv},
        {">="sv, "ge"sv},
        {"||"sv, "lor"sv},
        {"^^"sv, "lxor"sv},
        {"&&"sv, "land"sv},
        {"|"sv, "or"sv},
        {"^"sv, "xor"sv},
        {"&"sv, "and"sv},
        {"+"sv, "add"sv},
        {"-"sv, "sub"sv},
        {"*"sv, "mul"sv},
        {"/"sv, "div"sv},
        {"%"sv, "rem"sv},
        {"**"sv, "pow"sv},
        {"<<"sv, "shl"sv},
        {">>"sv, "shr"sv},
        {"++"sv, "inc"sv},
        {"--"sv, "dec"sv},
    };

    static const std::set assignment_map
    {
        "||="sv,
        "^^="sv,
        "&&="sv,
        "|="sv,
        "^="sv,
        "&="sv,
        "+="sv,
        "-="sv,
        "*="sv,
        "/="sv,
        "%="sv,
        "**="sv,
        "<<="sv,
        ">>="sv,
    };

    if (name == "=")
        return "copy";

    const auto is_assignment = assignment_map.contains(name);

    auto copy = name;
    if (is_assignment)
        copy.pop_back();

    if (!operator_map.contains(copy))
        Error("TODO");

    return std::string(operator_map.at(copy)) + (is_assignment ? ".copy" : "");
}

std::string NJS::Builder::GetUnaryOperatorName(const std::string &name)
{
    static const std::map<std::string_view, std::string_view> operator_map
    {
        {"++"sv, "inc"sv},
        {"--"sv, "dec"sv},
        {"*"sv, "deref"sv},
        {"&"sv, "ref"sv},
        {"-"sv, "neg"sv},
    };

    if (!operator_map.contains(name))
        Error("TODO");

    return std::string(operator_map.at(name));
}

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

    const auto prefix = module_id.empty() ? "" : module_id + '.';

    if (is_operator)
    {
        if (name == "()")
        {
            return prefix + "call." + std::to_string(parameters[0]->Info.GetHash());
        }
        if (parameters.size() == 1)
        {
            const auto operator_name = GetUnaryOperatorName(name);
            return prefix
                   + (is_var_arg ? std::string() : operator_name + '.')
                   + std::to_string(parameters[0]->Info.GetHash())
                   + (is_var_arg ? '.' + operator_name : std::string());
        }
        if (parameters.size() == 2)
        {
            const auto operator_name = GetBinaryOperatorName(name);
            return prefix
                   + std::to_string(parameters[0]->Info.GetHash())
                   + '.' + operator_name + '.'
                   + std::to_string(parameters[1]->Info.GetHash());
        }
        Error("TODO");
    }

    return prefix + name;
}
