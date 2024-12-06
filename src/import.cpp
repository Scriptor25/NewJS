#include <iostream>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Context.hpp>
#include <NJS/Import.hpp>
#include <NJS/Param.hpp>
#include <NJS/Value.hpp>

std::ostream& NJS::ImportMapping::Print(std::ostream& os) const
{
    if (!Name.empty() && SubMappings.empty())
        return os << Name;
    if (!Name.empty())
        os << Name << ": ";
    if (SubMappings.empty())
        return os << "{}";
    os << "{ ";
    bool first = true;
    for (const auto& [name_, mapping_] : SubMappings)
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
    const std::string& module_id,
    const std::vector<FunctionStmtPtr>& functions) const
{
    std::map<std::string, TypePtr> element_types;
    std::map<std::string, ValuePtr> elements;
    for (const auto& function : functions)
    {
        std::vector<TypePtr> param_types;
        for (const auto& param : function->Params)
            param_types.push_back(param->Type);
        const auto type = builder.GetCtx().GetFunctionType(param_types, function->ResultType, function->VarArg);
        element_types[function->Name] = type;
        auto callee = builder.GetModule().getOrInsertFunction(
            module_id + '.' + function->Name,
            type->GenFnLLVM(builder));
        elements[function->Name] = RValue::Create(builder, type, callee.getCallee());
    }

    if (!Name.empty() && SubMappings.empty())
    {
        const auto module_type = builder.GetCtx().GetObjectType(element_types);
        llvm::Value* module = llvm::Constant::getNullValue(module_type->GenLLVM(builder));
        for (const auto& [name_, value_] : elements)
            module = builder.GetBuilder().CreateInsertValue(module, value_->Load(), module_type->MemberIndex(name_));

        builder.DefVar(Name) = RValue::Create(builder, module_type, module);
    }
    else
    {
        for (const auto& [name_, value_] : elements)
        {
            if (!SubMappings.contains(name_)) continue;
            builder.DefVar(SubMappings.at(name_)) = value_;
        }
    }
}
