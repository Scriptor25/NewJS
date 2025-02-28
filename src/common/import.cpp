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


void NJS::ImportMapping::MapFunctions(
    Builder &builder,
    const std::string &module_id,
    const std::vector<FunctionStatementPtr> &functions) const
{
    std::vector<std::pair<std::string, TypePtr>> element_types;
    std::vector<std::pair<std::string, ValuePtr>> element_values;

    std::set<std::string> name_set;
    for (auto &name: NameMap | std::views::keys)
        name_set.insert(name);

    for (const auto &function: functions)
    {
        auto name = (function->Flags & FunctionFlags_Extern) ? std::string() : module_id + '.';
        if (function->Flags & FunctionFlags_Operator)
        {
            if (function->Parameters.size() == 1)
                name += (function->IsVarArg ? std::string() : function->Name)
                        + function->Parameters[0]->Info.GetString()
                        + (function->IsVarArg ? function->Name : std::string());
            else if (function->Parameters.size() == 2)
                name += function->Parameters[0]->Info.GetString()
                        + function->Name
                        + function->Parameters[1]->Info.GetString();
        }
        else
            name += function->Name;

        std::vector<ReferenceInfo> parameters;
        for (const auto &parameter: function->Parameters)
            parameters.emplace_back(parameter->Info);

        const auto type = builder.GetTypeContext().GetFunctionType(
            function->Result,
            parameters,
            function->IsVarArg);

        auto callee = builder.GetOrCreateFunction(type->GenFnLLVM(builder), llvm::GlobalValue::ExternalLinkage, name);

        if (function->Flags & FunctionFlags_Operator)
        {
            if (function->Parameters.size() == 1)
                builder.DefineOperator(
                    function->Name,
                    !function->IsVarArg,
                    function->Parameters[0]->Info,
                    function->Result,
                    callee.getCallee());
            else if (function->Parameters.size() == 2)
                builder.DefineOperator(
                    function->Name,
                    function->Parameters[0]->Info,
                    function->Parameters[1]->Info,
                    function->Result,
                    callee.getCallee());
            continue;
        }

        auto value = RValue::Create(builder, type, callee.getCallee());

        if (All)
        {
            builder.DefineVariable(function->Name, value);
            continue;
        }

        if (NameMap.contains(function->Name))
        {
            builder.DefineVariable(NameMap.at(function->Name), value);
            name_set.erase(function->Name);
            continue;
        }

        element_values.emplace_back(function->Name, value);
        element_types.emplace_back(function->Name, type);
    }

    if (!name_set.empty())
        return;

    if (Name.empty())
        return;

    const auto struct_type = builder.GetTypeContext().GetStructType(element_types);

    const auto type = struct_type->GetLLVM(builder);

    llvm::Value *value = llvm::Constant::getNullValue(type);
    for (const auto &[name_, value_]: element_values)
    {
        const auto [
            member_index_,
            member_name_,
            member_type_
        ] = struct_type->GetMember(name_);
        value = builder.GetBuilder().CreateInsertValue(value, value_->Load(), member_index_);
    }

    builder.DefineVariable(Name, RValue::Create(builder, struct_type, value));
}
