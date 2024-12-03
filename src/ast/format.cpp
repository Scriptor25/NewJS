#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Context.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Value.hpp>

NJS::FormatExpr::FormatExpr(
    const size_t count,
    std::map<size_t, std::string> statics,
    std::map<size_t, ExprPtr> dynamics)
    : Count(count), Statics(std::move(statics)), Dynamics(std::move(dynamics))
{
}

NJS::ValuePtr NJS::FormatExpr::GenLLVM(Builder& builder)
{
    constexpr auto N = 1024;
    const auto ptr = builder.CreateAlloca(builder.LLVMBuilder().getInt8Ty(), N);

    std::vector<llvm::Value*> args(3 + 2 * Statics.size() + 3 * Dynamics.size());
    size_t x = 0;
    args[x++] = ptr;
    args[x++] = builder.LLVMBuilder().getInt64(N);
    for (size_t i = 0; i < Count; ++i)
    {
        if (Statics.contains(i))
        {
            args[x++] = builder.LLVMBuilder().getInt32(1);

            const auto value = Statics[i];
            auto& str_ptr = ConstStringExpr::GlobalStringTable[value];
            if (!str_ptr) str_ptr = builder.LLVMBuilder().CreateGlobalStringPtr(value);
            args[x++] = str_ptr;
        }
        else if (Dynamics.contains(i))
        {
            args[x++] = builder.LLVMBuilder().getInt32(2);
            const auto value = Dynamics[i]->GenLLVM(builder);
            args[x++] = builder.LLVMBuilder().getInt32(value->GetType()->GetId());
            if (value->GetType()->IsComplex())
                args[x++] = value->GetPtr();
            else args[x++] = value->Load();
        }
    }
    args[x++] = builder.LLVMBuilder().getInt32(0);

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
        else if (Dynamics.contains(i)) Dynamics[i]->Print(os << '{') << '}';
    }
    return os << '"';
}
