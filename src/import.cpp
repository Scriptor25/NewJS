#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Import.hpp>
#include <NJS/Parameter.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

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

void NJS::ImportMapping::MapFunctions(
    Builder &builder,
    const SourceLocation &where,
    const std::string &module_id,
    const std::vector<FunctionStatementPtr> &functions) const
{
    std::vector<std::pair<std::string, TypePtr>> element_types;
    std::vector<std::pair<std::string, ValuePtr>> element_values;

    for (const auto &function: functions)
    {
        auto name = (function->Flags & FunctionFlags_Extern) ? std::string() : module_id + '.';
        if (function->Flags & FunctionFlags_Operator)
        {
            if (function->Parameters.size() == 1)
                name += (function->IsVarArg ? std::string() : function->Name)
                        + function->Parameters[0]->Type->GetString()
                        + (function->IsVarArg ? function->Name : std::string());
            else if (function->Parameters.size() == 2)
                name += function->Parameters[0]->Type->GetString()
                        + function->Name
                        + function->Parameters[1]->Type->GetString();
        }
        else
            name += function->Name;

        std::vector<TypePtr> parameter_types;
        for (const auto &parameter: function->Parameters)
            parameter_types.push_back(parameter->Type);

        const auto type = builder.GetTypeContext().GetFunctionType(
            function->ResultType,
            parameter_types,
            function->IsVarArg);

        auto function_callee = builder.GetModule().getFunction(name);
        if (!function_callee)
            function_callee = llvm::Function::Create(
                type->GenFnLLVM(where, builder),
                llvm::Function::ExternalLinkage,
                name,
                builder.GetModule());
        const auto value = RValue::Create(builder, type, function_callee);

        if (function->Flags & FunctionFlags_Operator)
        {
            if (function->Parameters.size() == 1)
                builder.DefineOperator(
                    function->Name,
                    !function->IsVarArg,
                    function->Parameters[0]->Type,
                    function->ResultType,
                    function_callee);
            else if (function->Parameters.size() == 2)
                builder.DefineOperator(
                    function->Name,
                    function->Parameters[0]->Type,
                    function->Parameters[1]->Type,
                    function->ResultType,
                    function_callee);
        }
        else if (All)
            builder.DefineVariable(where, function->Name) = value;
        else if (NameMap.contains(function->Name))
            builder.DefineVariable(where, NameMap.at(function->Name)) = value;
        else
        {
            element_values.emplace_back(function->Name, value);
            element_types.emplace_back(function->Name, type);
        }
    }

    if (!Name.empty())
    {
        const auto module_type = builder.GetTypeContext().GetStructType(element_types);
        llvm::Value *module = llvm::Constant::getNullValue(module_type->GetLLVM(where, builder));
        for (const auto &[name_, value_]: element_values)
            module = builder.GetBuilder().CreateInsertValue(
                module,
                value_->Load(where),
                module_type->GetMember(where, name_).Index);
        builder.DefineVariable(where, Name) = RValue::Create(builder, module_type, module);
    }
}
