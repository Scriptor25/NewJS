#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Value.hpp>

NJS::FormatExpr::FormatExpr(
    TypePtr type,
    const size_t count,
    std::map<size_t, std::string> statics,
    std::map<size_t, ExprPtr> dynamics)
    : Expr(std::move(type)), Count(count), Statics(std::move(statics)), Dynamics(std::move(dynamics))
{
}

NJS::ValuePtr NJS::FormatExpr::GenLLVM(Builder& builder)
{
    const auto ptr = builder.CreateAlloca(builder.LLVMBuilder().getInt8Ty(), 1024);

    std::vector<llvm::Value*> args(3 + Statics.size() + 2 * Dynamics.size());
    size_t x = 0;
    args[x++] = ptr;
    args[x++] = builder.LLVMBuilder().getInt64(Statics.size());
    args[x++] = builder.LLVMBuilder().getInt64(Dynamics.size());
    for (const auto& [i, s] : Statics)
        args[x++] = builder.LLVMBuilder().CreateLoad(
            builder.LLVMBuilder().getPtrTy(),
            builder.LLVMBuilder().CreateGlobalStringPtr(s));
    for (const auto& [i, d] : Dynamics)
    {
        const auto value = d->GenLLVM(builder);
        args[x++] = builder.LLVMBuilder().getInt64(0); // TODO: value type id
        args[x++] = value->Load();
    }

    llvm::FunctionCallee format;
    builder.GetFormat(format);
    builder.LLVMBuilder().CreateCall(format, args);

    return RValue::Create(builder, builder.Ctx().GetStringType(), ptr);
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
