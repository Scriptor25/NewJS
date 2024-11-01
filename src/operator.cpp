#include <map>
#include <NJS/Context.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Operator.hpp>

std::map<std::string, NJS::Operator> OPERATORS
{
    {"=", {"=", 0}},
    {"+=", {"=", 0}},
    {"-=", {"=", 0}},
    {"*=", {"=", 0}},
    {"/=", {"=", 0}},
    {"%=", {"=", 0}},
    {"&=", {"=", 0}},
    {"|=", {"=", 0}},
    {"^=", {"=", 0}},
    {"<<=", {"=", 0}},
    {">>=", {"=", 0}},
    {"||", {"b", 1}},
    {"^^", {"b", 2}},
    {"&&", {"b", 3}},
    {"|", {"n", 4}},
    {"^", {"n", 5}},
    {"&", {"n", 6}},
    {"==", {"b", 7}},
    {"!=", {"b", 7}},
    {"<", {"b", 8}},
    {"<=", {"b", 8}},
    {">", {"b", 8}},
    {">=", {"b", 8}},
    {"<<", {"n", 9}},
    {">>", {"n", 9}},
    {"+", {"n", 10}},
    {"-", {"n", 10}},
    {"*", {"n", 11}},
    {"/", {"n", 11}},
    {"%", {"n", 11}},
};

NJS::Operator& NJS::Operator::Get(const std::string& name)
{
    return OPERATORS[name];
}

NJS::TypePtr NJS::Operator::Result(Context& ctx, const TypePtr& lhs, const TypePtr& rhs) const
{
    if (Id == "=") return lhs;
    if (Id == "b") return ctx.GetBooleanType();
    if (Id == "n") return ctx.GetNumberType();
    Error("operator result is not defined");
}
