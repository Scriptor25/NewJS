#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Import.hpp>
#include <NJS/Param.hpp>
#include <NJS/Parser.hpp>
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

void NJS::ImportMapping::MapFunctions(Parser& parser, const std::vector<FunctionStmtPtr>& functions) const
{
    std::map<std::string, TypePtr> element_types;
    for (const auto& function : functions)
    {
        std::vector<TypePtr> arg_types;
        for (const auto& param : function->Args)
            arg_types.push_back(param->Type);
        const auto type = parser.m_Ctx.GetFunctionType(
            function->ResultType,
            arg_types,
            function->VarArg);
        element_types[function->Name] = type;
    }

    if (!Name.empty() && NameMap.empty())
    {
        const auto module_type = parser.m_Ctx.GetStructType(element_types);
        parser.DefVar(Name) = module_type;
        return;
    }

    for (const auto& [name_, type_] : element_types)
    {
        if (!NameMap.contains(name_)) continue;
        parser.DefVar(NameMap.at(name_)) = type_;
    }
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
        std::vector<TypePtr> arg_types;
        for (const auto& param : function->Args)
            arg_types.push_back(param->Type);
        const auto type = builder.GetCtx().GetFunctionType(
            function->ResultType,
            arg_types,
            function->VarArg);
        element_types[function->Name] = type;
        auto callee = builder.GetModule().getOrInsertFunction(
            module_id + '.' + function->Name,
            type->GenFnLLVM(builder));
        elements[function->Name] = RValue::Create(builder, type, callee.getCallee());
    }

    if (!Name.empty() && NameMap.empty())
    {
        const auto module_type = builder.GetCtx().GetStructType(element_types);
        llvm::Value* module = llvm::Constant::getNullValue(module_type->GetLLVM(builder));
        size_t i = 0;
        for (const auto& [name_, value_] : elements)
            module = builder.GetBuilder().CreateInsertValue(module, value_->Load(), i++);

        builder.DefVar(Name) = RValue::Create(builder, module_type, module);
        return;
    }

    for (const auto& [name_, value_] : elements)
    {
        if (!NameMap.contains(name_)) continue;
        builder.DefVar(NameMap.at(name_)) = value_;
    }
}
