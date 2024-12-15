#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>

NJS::ValuePtr NJS::Builder::CreateSubscript(const ValuePtr&, const size_t)
{
    Error("TODO");
}

NJS::ValuePtr NJS::Builder::CreateSubscript(const ValuePtr&, llvm::Value*)
{
    Error("TODO");
}
