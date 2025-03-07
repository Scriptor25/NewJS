#include <ranges>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/import.hpp>
#include <newjs/parameter.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

std::ostream &NJS::ImportMapping::Print(std::ostream &stream) const
{
    if (!Name.empty() && NameMap.empty())
        return stream << Name;
    if (!Name.empty())
        stream << Name << ": ";
    if (NameMap.empty())
        return stream << "{}";
    stream << "{ ";
    auto first = true;
    for (const auto &[name_, mapping_]: NameMap)
    {
        if (first)
            first = false;
        else
            stream << ", ";
        stream << name_;
        if (!mapping_.empty())
            stream << ": " << mapping_;
    }
    return stream << " }";
}

template<>
struct std::formatter<std::set<std::string>> final : formatter<string>
{
    template<typename FormatContext>
    auto format(const std::set<std::string> &values, FormatContext &ctx) const
    {
        std::string value_string;
        unsigned i = 0;
        for (auto &value: values)
        {
            if (i++ > 0)
                value_string += ", ";
            value_string += value;
        }
        return formatter<string>::format(value_string, ctx);
    }
};


void NJS::ImportMapping::MapValues(
    Builder &builder,
    const std::string &module_id,
    const std::vector<StatementPtr> &values) const
{
    std::vector<std::pair<std::string, ReferenceInfo>> element_infos;
    std::vector<std::pair<std::string, ValuePtr>> element_values;

    std::set<std::string> name_set;
    for (auto &name: NameMap | std::views::keys)
        name_set.insert(name);

    for (const auto &statement: values)
    {
        ValuePtr value;
        ReferenceInfo info;
        std::string name;

        if (auto function = std::dynamic_pointer_cast<FunctionStatement>(statement))
            MapFunction(builder, *function, module_id, value, name, info);
        else if (auto variable = std::dynamic_pointer_cast<VariableStatement>(statement))
            MapVariable(builder, *variable, module_id, value, name, info);

        if (!value)
            continue;

        if (All)
        {
            builder.DefineVariable(name, true) = value;
            continue;
        }

        if (NameMap.contains(name))
        {
            builder.DefineVariable(NameMap.at(name), true) = value;
            name_set.erase(name);
            continue;
        }

        element_values.emplace_back(name, value);
        element_infos.emplace_back(name, info);
    }

    if (!name_set.empty())
        return;

    if (Name.empty())
        return;

    const auto struct_type = builder.GetTypeContext().GetStructType(element_infos);
    const auto type = struct_type->GetLLVM(builder);

    llvm::Value *value = llvm::Constant::getNullValue(type);
    for (unsigned i = 0; i < element_values.size(); ++i)
    {
        const auto &element_value = element_values[i].second;
        const auto &[type_, is_const_, is_reference_] = element_infos[i].second;
        value = builder.GetBuilder().CreateInsertValue(
            value,
            is_reference_ ? element_value->GetPointer() : element_value->Load(),
            i);
    }

    builder.DefineVariable(Name, true) = RValue::Create(builder, struct_type, value);
}

void NJS::ImportMapping::MapFunction(
    Builder &builder,
    const FunctionStatement &function,
    const std::string &module_id,
    ValuePtr &value,
    std::string &name,
    ReferenceInfo &info)
{
    const bool is_extern = function.Flags & FunctionFlags_Extern;
    const bool is_operator = function.Flags & FunctionFlags_Operator;

    std::string function_name;
    if (is_extern)
    {
        function_name = function.Name;
    }
    else if (is_operator)
    {
        if (function.Parameters.size() == 1)
            function_name = module_id
                            + '.'
                            + (function.IsVarArg ? std::string() : function.Name)
                            + function.Parameters[0]->Info.GetString()
                            + (function.IsVarArg ? function.Name : std::string());
        else if (function.Parameters.size() == 2)
            function_name = module_id
                            + '.'
                            + function.Parameters[0]->Info.GetString()
                            + function.Name
                            + function.Parameters[1]->Info.GetString();
    }
    else
    {
        function_name = module_id + '.' + function.Name;
    }

    std::vector<ReferenceInfo> parameters;
    for (const auto &parameter: function.Parameters)
        parameters.emplace_back(parameter->Info);
    const auto type = builder.GetTypeContext().GetFunctionType(function.Result, parameters, function.IsVarArg);

    const auto fn = builder.GetOrCreateFunction(
        type->GenFnLLVM(builder),
        llvm::GlobalValue::ExternalLinkage,
        function_name).getCallee();

    if (is_operator)
    {
        if (function.Parameters.size() == 1)
            builder.DefineOperator(
                function.Name,
                !function.IsVarArg,
                function.Parameters[0]->Info,
                function.Result,
                fn);
        else if (function.Parameters.size() == 2)
            builder.DefineOperator(
                function.Name,
                function.Parameters[0]->Info,
                function.Parameters[1]->Info,
                function.Result,
                fn);
        return;
    }

    value = RValue::Create(builder, type, fn);
    info = {type, false, false};
    name = function.Name;
}

void NJS::ImportMapping::MapVariable(
    Builder &builder,
    const VariableStatement &variable,
    const std::string &module_id,
    ValuePtr &value,
    std::string &name,
    ReferenceInfo &info)
{
    auto is_const = variable.Parameter->Info.IsConst;
    auto type = variable.Parameter->Info.Type;

    info = {type, is_const, true};
    name = variable.Parameter->Name;

    std::string variable_name;
    if (variable.IsExtern)
        variable_name = name;
    else
        variable_name = module_id + '.' + name;

    value = builder.CreateGlobal(variable_name, type, is_const, false);
}
