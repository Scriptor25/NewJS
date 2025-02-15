#include <ranges>
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
    const std::vector<StatementPtr> &functions) const
{
    std::map<std::string, TypePtr> element_types;
    std::map<std::string, ValuePtr> element_values;

    for (const auto &function: functions)
    {
        const auto &fn = *std::dynamic_pointer_cast<FunctionStatement>(function);

        auto name = std::string(module_id) + '.';
        if (fn.Flags & FunctionFlags_Operator)
        {
            if (fn.Parameters.size() == 1)
                name += (fn.IsVarArg ? std::string() : fn.Name)
                        + fn.Parameters[0]->Type->GetString()
                        + (fn.IsVarArg ? fn.Name : std::string());
            else if (fn.Parameters.size() == 2)
                name += fn.Parameters[0]->Type->GetString() + fn.Name + fn.Parameters[1]->Type->GetString();
        }
        else
            name += fn.Name;

        std::vector<TypePtr> parameter_types;
        for (const auto &parameter: fn.Parameters)
            parameter_types.push_back(parameter->Type);

        const auto type = builder.GetTypeContext().GetFunctionType(
            fn.ResultType,
            parameter_types,
            fn.IsVarArg);

        auto callee = builder.GetModule().getOrInsertFunction(name, type->GenFnLLVM(where, builder));
        const auto value = RValue::Create(builder, type, callee.getCallee());

        if (fn.Flags & FunctionFlags_Operator)
        {
            if (fn.Parameters.size() == 1)
                builder.DefineOperator(
                    fn.Name,
                    !fn.IsVarArg,
                    fn.Parameters[0]->Type,
                    fn.ResultType,
                    callee.getCallee());
            else if (fn.Parameters.size() == 2)
                builder.DefineOperator(
                    fn.Name,
                    fn.Parameters[0]->Type,
                    fn.Parameters[1]->Type,
                    fn.ResultType,
                    callee.getCallee());
        }
        else if (All)
            builder.DefineVariable(where, fn.Name) = value;
        else if (NameMap.contains(fn.Name))
            builder.DefineVariable(where, NameMap.at(fn.Name)) = value;
        else
        {
            element_values[fn.Name] = value;
            element_types[fn.Name] = type;
        }
    }

    if (!Name.empty())
    {
        const auto module_type = builder.GetTypeContext().GetStructType(element_types);
        llvm::Value *module = llvm::Constant::getNullValue(module_type->GetLLVM(where, builder));
        unsigned i = 0;
        for (const auto &value_: element_values | std::ranges::views::values)
            module = builder.GetBuilder().CreateInsertValue(module, value_->Load(where), i++);
        builder.DefineVariable(where, Name) = RValue::Create(builder, module_type, module);
    }
}
