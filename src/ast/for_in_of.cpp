#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>

NJS::ForInOfStmt::ForInOfStmt(SourceLocation where, VariableStmt init, const bool of, ExprPtr value, StmtPtr body)
    : Stmt(std::move(where)), Init(std::move(init)), Of(of), Value(std::move(value)), Body(std::move(body))
{
}

NJS::ValuePtr NJS::ForInOfStmt::GenLLVM(Builder& builder)
{
    Error("TODO");
}

std::ostream& NJS::ForInOfStmt::Print(std::ostream& os)
{
    return Body->Print(Value->Print(Init.Print(os << "for (") << ' ' << (Of ? "of" : "in") << ' ') << ") ");
}
