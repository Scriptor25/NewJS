#include <ranges>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Import.hpp>
#include <NJS/Param.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

std::ostream& NJS::ImportMapping::Print(std::ostream& os) const
{
    if (!Name.empty() && NameMap.empty())
        return os << Name;
    if (!Name.empty())
        os << Name << ": ";
    if (NameMap.empty())
        return os << "{}";
    os << "{ ";
    bool first = true;
    for (const auto& [name_, mapping_] : NameMap)
    {
        if (first) first = false;
        else os << ", ";
        os << name_;
        if (!mapping_.empty())
            os << ": " << mapping_;
    }
    return os << " }";
}

void NJS::ImportMapping::MapFunctions(
    Builder& builder,
    const SourceLocation& where,
    const std::string& module_id,
    const std::vector<StmtPtr>& functions) const
{
    std::map<std::string, TypePtr> element_types;
    std::map<std::string, ValuePtr> elements;

    for (const auto& ptr : functions)
    {
        const auto& fn = *std::dynamic_pointer_cast<FunctionStmt>(ptr);

        std::string name;
        switch (fn.Fn)
        {
        case FnType_Function:
            name = module_id + '.' + fn.Name;
            break;
        case FnType_Operator:
            switch (fn.Args.size())
            {
            case 1:
                name = module_id + '.' + fn.Args[0]->Type->GetString() + fn.Name;
                break;
            case 2:
                name = module_id + '.' + fn.Args[0]->Type->GetString() + fn.Name + fn.Args[1]->Type->GetString();
                break;
            default:
                break;
            }
            break;
        default:
            continue;
        }

        std::vector<TypePtr> arg_types;
        for (const auto& param : fn.Args)
            arg_types.push_back(param->Type);

        const auto type = builder.GetCtx().GetFunctionType(
            fn.ResultType,
            arg_types,
            fn.VarArg);

        auto callee = builder.GetModule().getOrInsertFunction(name, type->GenFnLLVM(where, builder));
        const auto value = RValue::Create(builder, type, callee.getCallee());

        if (fn.Fn == FnType_Operator)
        {
            switch (fn.Args.size())
            {
            case 1:
                builder.DefOp(fn.Name, fn.Args[0]->Type, fn.ResultType, callee.getCallee());
                break;
            case 2:
                builder.DefOp(fn.Name, fn.Args[0]->Type, fn.Args[1]->Type, fn.ResultType, callee.getCallee());
                break;
            default:
                break;
            }
        }
        else if (All)
        {
            builder.DefVar(where, fn.Name) = value;
        }
        else if (NameMap.contains(fn.Name))
        {
            builder.DefVar(where, NameMap.at(fn.Name)) = value;
        }
        else
        {
            elements[fn.Name] = value;
            element_types[fn.Name] = type;
        }
    }

    if (!Name.empty())
    {
        const auto module_type = builder.GetCtx().GetStructType(element_types);
        llvm::Value* module = llvm::Constant::getNullValue(module_type->GetLLVM(where, builder));
        unsigned i = 0;
        for (const auto& value_ : elements | std::ranges::views::values)
            module = builder.GetBuilder().CreateInsertValue(module, value_->Load(where), i++);

        builder.DefVar(where, Name) = RValue::Create(builder, module_type, module);
    }
}
