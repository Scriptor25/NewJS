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
    bool first = true;
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
    const std::string_view &module_id,
    const std::vector<StatementPtr> &functions) const
{
    std::map<std::string, TypePtr> element_types;
    std::map<std::string, ValuePtr> elements;

    for (const auto &ptr: functions)
    {
        const auto &fn = *std::dynamic_pointer_cast<FunctionStatement>(ptr);

        std::string name;
        switch (fn.FnID)
        {
            case FnType_Function:
                name = std::string(module_id) + '.' + fn.Name;
                break;
            case FnType_Operator:
                switch (fn.Args.size())
                {
                    case 1:
                        name = std::string(module_id) + '.' + fn.Args[0]->Type->GetString() + fn.Name;
                        break;
                    case 2:
                        name = std::string(module_id) + '.' + fn.Args[0]->Type->GetString() + fn.Name
                               + fn.Args[1]->Type->GetString();
                        break;
                    default:
                        break;
                }
                break;
            default:
                continue;
        }

        std::vector<TypePtr> arg_types;
        for (const auto &param: fn.Args)
            arg_types.push_back(param->Type);

        const auto type = builder.GetTypeContext().GetFunctionType(
            fn.ResultType,
            arg_types,
            fn.VarArg);

        auto callee = builder.GetModule().getOrInsertFunction(name, type->GenFnLLVM(where, builder));
        const auto value = RValue::Create(builder, type, callee.getCallee());

        if (fn.FnID == FnType_Operator)
        {
            switch (fn.Args.size())
            {
                case 1:
                    builder.DefineOperator(fn.Name, fn.Args[0]->Type, fn.ResultType, callee.getCallee());
                    break;
                case 2:
                    builder.DefineOperator(fn.Name, fn.Args[0]->Type, fn.Args[1]->Type, fn.ResultType, callee.getCallee());
                    break;
                default:
                    break;
            }
        }
        else if (All)
        {
            builder.DefineVariable(where, fn.Name) = value;
        }
        else if (NameMap.contains(fn.Name))
        {
            builder.DefineVariable(where, NameMap.at(fn.Name)) = value;
        }
        else
        {
            elements[fn.Name] = value;
            element_types[fn.Name] = type;
        }
    }

    if (!Name.empty())
    {
        const auto module_type = builder.GetTypeContext().GetStructType(element_types);
        llvm::Value *module = llvm::Constant::getNullValue(module_type->GetLLVM(where, builder));
        unsigned i = 0;
        for (const auto &value_: elements | std::ranges::views::values)
            module = builder.GetBuilder().CreateInsertValue(module, value_->Load(where), i++);

        builder.DefineVariable(where, Name) = RValue::Create(builder, module_type, module);
    }
}
