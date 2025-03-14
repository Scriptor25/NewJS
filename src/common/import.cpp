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
    const std::vector<ExportStatementPtr> &exports) const
{
    std::vector<StructElement> elements;
    std::vector<std::pair<std::string, ValuePtr>> element_values;

    std::set<std::string> name_set;
    for (auto &name: NameMap | std::views::keys)
        name_set.insert(name);

    for (const auto &export_: exports)
    {
        ValuePtr value;
        ReferenceInfo info;
        std::string name;

        export_->Value->GenImport(builder, module_id, value, info, name);

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
        elements.emplace_back(name, info, nullptr);
    }

    if (!name_set.empty())
        Error("missing symbols in import mapping: {}", name_set);

    if (Name.empty())
        return;

    const auto struct_type = builder.GetTypeContext().GetStructType(elements, "module." + module_id);
    const auto type = struct_type->GetLLVM(builder);

    llvm::Value *value = llvm::Constant::getNullValue(type);
    for (unsigned i = 0; i < element_values.size(); ++i)
    {
        const auto &element_value = element_values[i].second;
        const auto &[type_, is_const_, is_reference_] = elements[i].Info;
        value = builder.GetBuilder().CreateInsertValue(
            value,
            is_reference_ ? element_value->GetPointer() : element_value->Load(),
            i);
    }

    builder.DefineVariable(Name, true) = RValue::Create(builder, struct_type, value);
}
