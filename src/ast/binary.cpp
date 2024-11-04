#include <llvm/IR/Value.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Value.hpp>

NJS::BinaryExpr::BinaryExpr(TypePtr type, std::string op, ExprPtr lhs, ExprPtr rhs)
    : Expr(std::move(type)), Op(std::move(op)), LHS(std::move(lhs)), RHS(std::move(rhs))
{
}

NJS::ValuePtr NJS::BinaryExpr::GenLLVM(Builder& builder)
{
    auto lhs = LHS->GenLLVM(builder);
    const auto rhs = RHS->GenLLVM(builder);

    if (Op == "=")
    {
        lhs->Store(rhs->Load());
        return lhs;
    }

    llvm::Value* value{};

    const auto lv = lhs->Load();
    const auto rv = rhs->Load();

    if (Op == "<") value = builder.LLVMBuilder().CreateFCmpOLT(lv, rv);

    if (value)
        return RValue::Create(builder, builder.Ctx().GetBooleanType(), value);

    const auto assign = Op.back() == '=';
    const auto op = assign ? Op.substr(0, Op.size() - 1) : Op;
    if (op == "+") value = builder.LLVMBuilder().CreateFAdd(lv, rv);
    if (op == "-") value = builder.LLVMBuilder().CreateFSub(lv, rv);

    if (value)
    {
        if (assign)
        {
            lhs->Store(value);
            return lhs;
        }
        return RValue::Create(builder, builder.Ctx().GetNumberType(), value);
    }

    Error("undefined binary operator");
}

std::ostream& NJS::BinaryExpr::Print(std::ostream& os)
{
    return os << LHS << ' ' << Op << ' ' << RHS;
}
