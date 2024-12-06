#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Context.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Std.hpp>
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
    const auto ptr = builder.CreateAlloca(builder.GetBuilder().getInt8Ty(), N);

    std::vector<llvm::Value*> args;

    args.push_back(ptr);
    args.push_back(builder.GetBuilder().getInt64(N));

    for (size_t i = 0; i < Count; ++i)
    {
        if (Statics.contains(i))
        {
            const auto value = Statics[i];
            const auto str = ConstStringExpr::GetString(builder, value);

            args.push_back(builder.GetBuilder().getInt32(ID_STRING));
            args.push_back(str);
        }
        else if (Dynamics.contains(i))
        {
            const auto value = Dynamics[i]->GenLLVM(builder);
            value->GetType()->TypeInfo(builder, args);
            if (value->GetType()->IsPrimitive())
                args.push_back(value->Load());
            else if (value->IsL())
                args.push_back(value->GetPtr());
            else
            {
                const auto tmp = builder.CreateAlloca(value->GetType());
                tmp->Store(value);
                args.push_back(tmp->GetPtr());
            }
        }
    }

    args.push_back(builder.GetBuilder().getInt32(ID_VOID));

    llvm::FunctionCallee format;
    builder.GetFormat(format);
    builder.GetBuilder().CreateCall(format, args);

    return RValue::Create(builder, builder.GetCtx().GetStringType(), ptr);
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
