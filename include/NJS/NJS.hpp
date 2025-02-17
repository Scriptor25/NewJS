#pragma once

#include <format>
#include <memory>

namespace NJS
{
    class TypeContext;
    class TemplateContext;

    class Parser;
    class Builder;
    class Linker;

    struct SourceLocation;
    struct Token;

    using TypePtr = std::shared_ptr<class Type>;
    using NoTypePtr = std::shared_ptr<class NoType>;
    using VoidTypePtr = std::shared_ptr<class VoidType>;
    using IntegerTypePtr = std::shared_ptr<class IntegerType>;
    using FloatingPointTypePtr = std::shared_ptr<class FloatingPointType>;
    using PointerTypePtr = std::shared_ptr<class PointerType>;
    using ReferenceTypePtr = std::shared_ptr<class ReferenceType>;
    using ArrayTypePtr = std::shared_ptr<class ArrayType>;
    using StructTypePtr = std::shared_ptr<class StructType>;
    using TupleTypePtr = std::shared_ptr<class TupleType>;
    using FunctionTypePtr = std::shared_ptr<class FunctionType>;

    using ParameterPtr = std::shared_ptr<struct Parameter>;
    using ValuePtr = std::shared_ptr<class Value>;

    using StatementPtr = std::shared_ptr<struct Statement>;
    using ExpressionPtr = std::shared_ptr<struct Expression>;

    void Indent();
    void Exdent();

    std::ostream &Spacing(std::ostream &stream);

    std::string TypeString(const TypePtr &type);
}

template<>
struct std::formatter<NJS::TypePtr> final : formatter<string>
{
    template<typename FormatContext>
    auto format(const NJS::TypePtr &type, FormatContext &ctx) const
    {
        return formatter<string>::format(TypeString(type), ctx);
    }
};
