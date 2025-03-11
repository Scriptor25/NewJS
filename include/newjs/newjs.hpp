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

    class Type;
    class IncompleteType;
    class VoidType;
    class IntegerType;
    class FloatingPointType;
    class PointerType;
    class ArrayType;
    class StructType;
    class TupleType;
    class FunctionType;

    using TypePtr = std::shared_ptr<Type>;
    using IncompleteTypePtr = std::shared_ptr<IncompleteType>;
    using VoidTypePtr = std::shared_ptr<VoidType>;
    using IntegerTypePtr = std::shared_ptr<IntegerType>;
    using FloatingPointTypePtr = std::shared_ptr<FloatingPointType>;
    using PointerTypePtr = std::shared_ptr<PointerType>;
    using ArrayTypePtr = std::shared_ptr<ArrayType>;
    using StructTypePtr = std::shared_ptr<StructType>;
    using TupleTypePtr = std::shared_ptr<TupleType>;
    using FunctionTypePtr = std::shared_ptr<FunctionType>;

    struct Parameter;
    struct DestructureTuple;
    struct DestructureStruct;

    using ParameterPtr = std::shared_ptr<Parameter>;

    class Value;
    class LValue;
    class RValue;

    using ValuePtr = std::shared_ptr<Value>;

    struct Statement;
    struct ForStatement;
    struct FunctionStatement;
    struct IfStatement;
    struct ImportStatement;
    struct ReturnStatement;
    struct ScopeStatement;
    struct SwitchStatement;
    struct VariableStatement;
    struct WhileStatement;

    using StatementPtr = std::shared_ptr<Statement>;
    using FunctionStatementPtr = std::shared_ptr<FunctionStatement>;

    struct Expression;
    struct BinaryExpression;
    struct BooleanExpression;
    struct CallExpression;
    struct CastExpression;
    struct CharExpression;
    struct FormatExpression;
    struct FloatingPointExpression;
    struct FunctionExpression;
    struct IntegerExpression;
    struct MemberExpression;
    struct ScopeExpression;
    struct SizeOfExpression;
    struct StringExpression;
    struct StructExpression;
    struct SubscriptExpression;
    struct SwitchExpression;
    struct SymbolExpression;
    struct TernaryExpression;
    struct TupleExpression;
    struct TypeOfExpression;
    struct UnaryExpression;

    using ExpressionPtr = std::shared_ptr<Expression>;

    enum FunctionFlags
    {
        FunctionFlags_None = 0,
        FunctionFlags_Extern = 1 << 0,
        FunctionFlags_Export = 1 << 1,
        FunctionFlags_Operator = 1 << 2,
        FunctionFlags_Template = 1 << 3,
        FunctionFlags_Absolute = 1 << 4,
    };

    struct ValueInfo;
    struct ReferenceInfo;
    struct MemberInfo;

    template<unsigned N>
    struct OperatorInfo;

    void Indent();
    void Exdent();

    std::ostream &Spacing(std::ostream &stream);

    std::string TypeString(const TypePtr &type);

    bool operator==(TypePtr a, TypePtr b);
    bool operator!=(const TypePtr &a, const TypePtr &b);
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
