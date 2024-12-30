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

    typedef std::shared_ptr<class Type> TypePtr;
    typedef std::shared_ptr<class NoType> NoTypePtr;
    typedef std::shared_ptr<class VoidType> VoidTypePtr;
    typedef std::shared_ptr<class IntType> IntTypePtr;
    typedef std::shared_ptr<class FPType> FPTypePtr;
    typedef std::shared_ptr<class PtrType> PtrTypePtr;
    typedef std::shared_ptr<class RefType> RefTypePtr;
    typedef std::shared_ptr<class ArrayType> ArrayTypePtr;
    typedef std::shared_ptr<class StructType> StructTypePtr;
    typedef std::shared_ptr<class TupleType> TupleTypePtr;
    typedef std::shared_ptr<class FunctionType> FunctionTypePtr;

    typedef std::shared_ptr<struct Param> ParamPtr;
    typedef std::shared_ptr<class Value> ValuePtr;

    typedef std::shared_ptr<struct Stmt> StmtPtr;
    typedef std::shared_ptr<struct Expr> ExprPtr;

    void Indent();
    void Exdent();

    std::ostream& Spacing(std::ostream&);

    std::string TypeString(const TypePtr&);
}

template <>
struct std::formatter<NJS::TypePtr> : formatter<string>
{
    template <typename FormatContext>
    auto format(const NJS::TypePtr& type, FormatContext& ctx) const
    {
        return formatter<string>::format(TypeString(type), ctx);
    }
};
