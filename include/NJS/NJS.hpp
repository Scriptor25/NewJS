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

    struct UnaryResult;

    typedef std::shared_ptr<class Type> TypePtr;
    typedef std::shared_ptr<class NoType> NoTypePtr;
    typedef std::shared_ptr<class VoidType> VoidTypePtr;
    typedef std::shared_ptr<class IntegerType> IntegerTypePtr;
    typedef std::shared_ptr<class FloatingPointType> FloatingPointTypePtr;
    typedef std::shared_ptr<class PointerType> PointerTypePtr;
    typedef std::shared_ptr<class ReferenceType> ReferenceTypePtr;
    typedef std::shared_ptr<class ArrayType> ArrayTypePtr;
    typedef std::shared_ptr<class StructType> StructTypePtr;
    typedef std::shared_ptr<class TupleType> TupleTypePtr;
    typedef std::shared_ptr<class FunctionType> FunctionTypePtr;

    typedef std::shared_ptr<struct Parameter> ParameterPtr;
    typedef std::shared_ptr<class Value> ValuePtr;

    typedef std::shared_ptr<struct Statement> StatementPtr;
    typedef std::shared_ptr<struct Expression> ExpressionPtr;

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
