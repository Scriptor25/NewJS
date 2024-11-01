#include <NJS/AST.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Param.hpp>

std::ostream& NJS::operator<<(std::ostream& os, const StmtPtr& ptr)
{
    return ptr->Print(os);
}

std::ostream& NJS::operator<<(std::ostream& os, const ExprPtr& ptr)
{
    return ptr->Print(os);
}

std::ostream& NJS::ScopeStmt::Print(std::ostream& os)
{
    if (Children.empty()) return os << "{}";

    os << "{" << std::endl;
    Indent();
    for (const auto& child : Children)
        Spacing(os) << child << std::endl;
    Exdent();
    return Spacing(os) << "}";
}

std::ostream& NJS::FunctionStmt::Print(std::ostream& os)
{
    os << "function " << Name << "(";
    for (size_t i = 0; i < Params.size(); ++i)
    {
        if (i > 0) os << ", ";
        os << Params[i];
    }
    return Body.Print(os << "): " << ResultType << ' ');
}

std::ostream& NJS::VariableStmt::Print(std::ostream& os)
{
    os << (IsConst ? "const" : "let") << ' ' << Name;
    if (Value) os << " = " << Value;
    return os;
}

std::ostream& NJS::IfStmt::Print(std::ostream& os)
{
    os << "if (" << Condition << ") " << Then;
    if (Else) os << " else " << Else;
    return os;
}

std::ostream& NJS::ForStmt::Print(std::ostream& os)
{
    os << "for (";
    if (Init) os << Init;
    os << ";";
    if (Condition) os << ' ' << Condition;
    os << ";";
    if (Loop) os << ' ' << Loop;
    return os << ") " << Body;
}

std::ostream& NJS::ForInOfStmt::Print(std::ostream& os)
{
    return os << "for (" << Init << ' ' << (Of ? "of" : "in") << ' ' << Value << ") " << Body;
}

std::ostream& NJS::ReturnStmt::Print(std::ostream& os)
{
    if (Value)
        return os << "return " << Value;
    return os << "return";
}

std::ostream& NJS::BinaryExpr::Print(std::ostream& os)
{
    return os << LHS << ' ' << Op << ' ' << RHS;
}

std::ostream& NJS::UnaryExpr::Print(std::ostream& os)
{
    return os << (OpRight ? "" : Op) << Operand << (OpRight ? Op : "");
}

std::ostream& NJS::MemberExpr::Print(std::ostream& os)
{
    return os << Object << '.' << Member;
}

std::ostream& NJS::CallExpr::Print(std::ostream& os)
{
    os << Callee << '(';
    for (size_t i = 0; i < Args.size(); ++i)
    {
        if (i > 0) os << ", ";
        os << Args[i];
    }
    return os << ')';
}

std::ostream& NJS::SubscriptExpr::Print(std::ostream& os)
{
    return os << Array << '[' << Index << ']';
}

std::ostream& NJS::SymbolExpr::Print(std::ostream& os)
{
    return os << Name;
}

std::ostream& NJS::NumberExpr::Print(std::ostream& os)
{
    return os << Value;
}

std::ostream& NJS::StringExpr::Print(std::ostream& os)
{
    return os << '"' << Value << '"';
}

std::ostream& NJS::ObjectExpr::Print(std::ostream& os)
{
    if (Entries.empty()) return os << "{}";

    os << '{' << std::endl;
    Indent();
    for (const auto& [name, value] : Entries)
        Spacing(os) << name << ": " << value << ',' << std::endl;
    Exdent();
    return Spacing(os) << '}';
}

std::ostream& NJS::ArrayExpr::Print(std::ostream& os)
{
    if (Entries.empty()) return os << "[]";

    os << '[' << std::endl;
    Indent();
    for (const auto& entry : Entries)
        Spacing(os) << entry << ',' << std::endl;
    Exdent();
    return Spacing(os) << ']';
}

std::ostream& NJS::FormatExpr::Print(std::ostream& os)
{
    os << "$\"";
    for (size_t i = 0; i < Count; ++i)
    {
        if (Statics.contains(i)) os << Statics[i];
        else if (Dynamics.contains(i)) os << '{' << Dynamics[i] << '}';
    }
    return os << '"';
}

std::ostream& NJS::FunctionExpr::Print(std::ostream& os)
{
    os << '?';
    if (!Params.empty())
    {
        os << '(';
        for (size_t i = 0; i < Params.size(); ++i)
        {
            if (i > 0) os << ", ";
            os << Params[i];
        }
        os << ") ";
    }
    return Body.Print(os);
}
