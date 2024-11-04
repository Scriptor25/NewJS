#include <NJS/AST.hpp>

NJS::ForInOfStmt::ForInOfStmt(StmtPtr init, const bool of, ExprPtr value, StmtPtr body)
    : Init(std::move(init)), Of(of), Value(std::move(value)), Body(std::move(body))
{
}

NJS::ValuePtr NJS::ForInOfStmt::GenLLVM(Builder& builder)
{
}

std::ostream& NJS::ForInOfStmt::Print(std::ostream& os)
{
    return os << "for (" << Init << ' ' << (Of ? "of" : "in") << ' ' << Value << ") " << Body;
}
