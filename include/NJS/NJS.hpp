#pragma once

#include <format>
#include <memory>

namespace NJS
{
    class TypeContext;
    class Parser;
    class Builder;
    class Linker;

    struct ModuleInfo;

    struct SourceLocation;
    struct Token;

    typedef std::shared_ptr<struct Type> TypePtr;
    typedef std::shared_ptr<struct NoType> NoTypePtr;
    typedef std::shared_ptr<struct PrimitiveType> PrimitiveTypePtr;
    typedef std::shared_ptr<struct ArrayType> ArrayTypePtr;
    typedef std::shared_ptr<struct TupleType> TupleTypePtr;
    typedef std::shared_ptr<struct ObjectType> ObjectTypePtr;
    typedef std::shared_ptr<struct FunctionType> FunctionTypePtr;
    typedef std::shared_ptr<struct VectorType> VectorTypePtr;

    enum Primitive
    {
        Primitive_Void,
        Primitive_Boolean,
        Primitive_Number,
        Primitive_String,
        Primitive_Char,
    };

    typedef std::shared_ptr<struct Param> ParamPtr;
    typedef std::shared_ptr<class Value> ValuePtr;

    typedef std::shared_ptr<struct Stmt> StmtPtr;
    typedef std::shared_ptr<struct Expr> ExprPtr;
    typedef std::shared_ptr<struct ImportStmt> ImportStmtPtr;
    typedef std::shared_ptr<struct ScopeStmt> ScopeStmtPtr;
    typedef std::shared_ptr<struct FunctionStmt> FunctionStmtPtr;
    typedef std::shared_ptr<struct VariableStmt> VariableStmtPtr;
    typedef std::shared_ptr<struct IfStmt> IfStmtPtr;
    typedef std::shared_ptr<struct ForStmt> ForStmtPtr;
    typedef std::shared_ptr<struct ForInOfStmt> ForInOfStmtPtr;
    typedef std::shared_ptr<struct ReturnStmt> ReturnStmtPtr;

    void Indent();
    void Exdent();

    std::ostream& Spacing(std::ostream&);

    std::string TypeString(const TypePtr&);
}

namespace std
{
    template <>
    struct formatter<NJS::TypePtr> : formatter<string>
    {
        template <typename FormatContext>
        auto format(const NJS::TypePtr& type, FormatContext& ctx) const
        {
            return formatter<string>::format(TypeString(type), ctx);
        }
    };
}
