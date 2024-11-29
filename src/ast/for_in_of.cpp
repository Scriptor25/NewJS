#include <NJS/AST.hpp>
#include <NJS/Error.hpp>

NJS::ForInOfStmt::ForInOfStmt(StmtPtr init, const bool of, ExprPtr value, StmtPtr body)
    : Init(std::move(init)), Of(of), Value(std::move(value)), Body(std::move(body))
{
}

NJS::ValuePtr NJS::ForInOfStmt::GenLLVM(Builder& builder)
{
    Error("NJS::ForInOfStmt::GenLLVM");
}

std::ostream& NJS::ForInOfStmt::Print(std::ostream& os)
{
    return Body->Print(Value->Print(Init->Print(os << "for (") << ' ' << (Of ? "of" : "in") << ' ') << ") ");
}
