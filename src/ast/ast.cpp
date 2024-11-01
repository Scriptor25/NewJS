#include <NJS/AST.hpp>

NJS::ScopeStmt::ScopeStmt(std::vector<StmtPtr> children)
    : Children(std::move(children))
{
}

NJS::FunctionStmt::FunctionStmt(std::string name, std::vector<ParamPtr> params, TypePtr result_type, ScopeStmt body)
    : Name(std::move(name)), Params(std::move(params)), ResultType(std::move(result_type)), Body(std::move(body))
{
}

NJS::VariableStmt::VariableStmt(const bool is_const, ParamPtr name, ExprPtr value)
    : IsConst(is_const), Name(std::move(name)), Value(std::move(value))
{
}

NJS::IfStmt::IfStmt(ExprPtr condition, StmtPtr then, StmtPtr else_)
    : Condition(std::move(condition)), Then(std::move(then)), Else(std::move(else_))
{
}

NJS::ForStmt::ForStmt(StmtPtr init, ExprPtr condition, StmtPtr loop, StmtPtr body)
    : Init(std::move(init)), Condition(std::move(condition)), Loop(std::move(loop)), Body(std::move(body))
{
}

NJS::ForInOfStmt::ForInOfStmt(StmtPtr init, const bool of, ExprPtr value, StmtPtr body)
    : Init(std::move(init)), Of(of), Value(std::move(value)), Body(std::move(body))
{
}

NJS::ReturnStmt::ReturnStmt(ExprPtr value)
    : Value(std::move(value))
{
}

NJS::BinaryExpr::BinaryExpr(TypePtr type, std::string op, ExprPtr lhs, ExprPtr rhs)
    : Expr(std::move(type)), Op(std::move(op)), LHS(std::move(lhs)), RHS(std::move(rhs))
{
}

NJS::UnaryExpr::UnaryExpr(TypePtr type, std::string op, const bool op_right, ExprPtr operand)
    : Expr(std::move(type)), Op(std::move(op)), OpRight(op_right), Operand(std::move(operand))
{
}

NJS::MemberExpr::MemberExpr(TypePtr type, ExprPtr object, std::string member)
    : Expr(std::move(type)), Object(std::move(object)), Member(std::move(member))
{
}

NJS::CallExpr::CallExpr(TypePtr type, ExprPtr callee, std::vector<ExprPtr> args)
    : Expr(std::move(type)), Callee(std::move(callee)), Args(std::move(args))
{
}

NJS::SubscriptExpr::SubscriptExpr(TypePtr type, ExprPtr array, ExprPtr index)
    : Expr(std::move(type)), Array(std::move(array)), Index(std::move(index))
{
}

NJS::SymbolExpr::SymbolExpr(TypePtr type, std::string name)
    : Expr(std::move(type)), Name(std::move(name))
{
}

NJS::NumberExpr::NumberExpr(TypePtr type, const double value)
    : Expr(std::move(type)), Value(value)
{
}

NJS::StringExpr::StringExpr(TypePtr type, std::string value)
    : Expr(std::move(type)), Value(std::move(value))
{
}

NJS::ObjectExpr::ObjectExpr(TypePtr type, std::map<std::string, ExprPtr> entries)
    : Expr(std::move(type)), Entries(std::move(entries))
{
}

NJS::Expr::Expr(TypePtr type)
    : Type(std::move(type))
{
}

NJS::ArrayExpr::ArrayExpr(TypePtr type, std::vector<ExprPtr> entries)
    : Expr(std::move(type)), Entries(std::move(entries))
{
}

NJS::FormatExpr::FormatExpr(
    TypePtr type,
    const size_t count,
    std::map<size_t, std::string> statics,
    std::map<size_t, ExprPtr> dynamics)
    : Expr(std::move(type)), Count(count), Statics(std::move(statics)), Dynamics(std::move(dynamics))
{
}

NJS::FunctionExpr::FunctionExpr(TypePtr type, std::vector<ParamPtr> params, ScopeStmt body)
    : Expr(std::move(type)), Params(std::move(params)), Body(std::move(body))
{
}
