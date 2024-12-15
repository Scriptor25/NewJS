#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Std.hpp>
#include <NJS/Type.hpp>

std::string NJS::ArrayType::GenString(const TypePtr& element, const unsigned count)
{
    return element->GetString() + '[' + std::to_string(count) + ']';
}

bool NJS::ArrayType::IsArray() const
{
    return true;
}

void NJS::ArrayType::TypeInfo(Builder& builder, std::vector<llvm::Value*>& args) const
{
    args.push_back(builder.GetBuilder().getInt32(ID_ARRAY));
    args.push_back(builder.GetBuilder().getInt32(m_Count));
    m_Element->TypeInfo(builder, args);
}

NJS::ArrayType::ArrayType(
    TypeContext& ctx,
    std::string string,
    TypePtr element,
    const unsigned count)
    : Type(ctx, std::move(string)), m_Element(std::move(element)), m_Count(count)
{
}

llvm::Type* NJS::ArrayType::GenLLVM(const Builder& builder) const
{
    return llvm::ArrayType::get(m_Element->GetLLVM(builder), m_Count);
}

unsigned NJS::ArrayType::GenSize() const
{
    return m_Element->GetSize() * m_Count;
}
