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

NJS::TypePtr NJS::ArrayType::GetElement() const
{
    return m_Element;
}

NJS::TypePtr NJS::ArrayType::GetElement(const unsigned i) const
{
    if (i >= m_Count)
        Error("array index out of bounds: {} !E [0,{})", i, m_Count);
    return m_Element;
}

void NJS::ArrayType::TypeInfo(const SourceLocation& where, Builder& builder, std::vector<llvm::Value*>& args) const
{
    args.push_back(builder.GetBuilder().getInt32(ID_ARRAY));
    args.push_back(builder.GetBuilder().getInt32(m_Count));
    m_Element->TypeInfo(where, builder, args);
}

NJS::ArrayType::ArrayType(
    TypeContext& ctx,
    std::string string,
    TypePtr element,
    const unsigned count)
    : Type(ctx, std::move(string)), m_Element(std::move(element)), m_Count(count)
{
}

llvm::Type* NJS::ArrayType::GenLLVM(const SourceLocation& where, const Builder& builder) const
{
    return llvm::ArrayType::get(m_Element->GetLLVM(where, builder), m_Count);
}

unsigned NJS::ArrayType::GenSize() const
{
    return m_Element->GetSize() * m_Count;
}
