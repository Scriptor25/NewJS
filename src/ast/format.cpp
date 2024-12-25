#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Std.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::FormatExpr::FormatExpr(
    SourceLocation where,
    const unsigned count,
    std::map<unsigned, std::string> statics,
    std::map<unsigned, ExprPtr> dynamics)
    : Expr(std::move(where)), Count(count), Statics(std::move(statics)), Dynamics(std::move(dynamics))
{
}

NJS::ValuePtr NJS::FormatExpr::GenLLVM(Builder& builder, const TypePtr&)
{
    constexpr auto N = 1024;
    const auto ptr = builder.CreateAlloca(builder.GetBuilder().getInt8Ty(), N);

    std::vector<llvm::Value*> args;

    args.push_back(ptr);
    args.push_back(builder.GetBuilder().getInt64(N));

    for (unsigned i = 0; i < Count; ++i)
    {
        if (Statics.contains(i))
        {
            const auto value = Statics[i];
            const auto str = StringExpr::GetString(builder, value);

            args.push_back(builder.GetBuilder().getInt32(ID_POINTER));
            args.push_back(builder.GetBuilder().getInt32(ID_INT));
            args.push_back(builder.GetBuilder().getInt32(8));
            args.push_back(builder.GetBuilder().getInt32(1));
            args.push_back(str);
        }
        else if (Dynamics.contains(i))
        {
            const auto value = Dynamics[i]->GenLLVM(builder, {});
            value->GetType()->TypeInfo(builder, args);
            if (value->GetType()->IsPrimitive())
                args.push_back(value->Load());
            else if (value->IsL())
                args.push_back(value->GetPtr(Where));
            else
            {
                const auto tmp = builder.CreateAlloca(value->GetType());
                tmp->Store(Where, value);
                args.push_back(tmp->GetPtr(Where));
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
    for (unsigned i = 0; i < Count; ++i)
    {
        if (Statics.contains(i)) os << Statics[i];
        else if (Dynamics.contains(i)) Dynamics[i]->Print(os << '{') << '}';
    }
    return os << '"';
}
