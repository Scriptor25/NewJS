#include <utility>
#include <llvm/IR/Value.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Value.hpp>

NJS::UnaryExpr::UnaryExpr(SourceLocation where, TypePtr type, std::string op, const bool op_right, ExprPtr operand)
    : Expr(std::move(where), std::move(type)), Op(std::move(op)), OpRight(op_right), Operand(std::move(operand))
{
}

NJS::ValuePtr NJS::UnaryExpr::GenLLVM(Builder& builder)
{
    auto operand = Operand->GenLLVM(builder);

    const auto ov = operand->Load();
    const auto one = llvm::ConstantFP::get(builder.GetBuilder().getDoubleTy(), 1.0);

    llvm::Value* value{};
    bool assign = false;
    if (Op == "++") value = builder.GetBuilder().CreateFAdd(ov, one), assign = true;
    else if (Op == "--") value = builder.GetBuilder().CreateFSub(ov, one), assign = true;
    else if (Op == "-") value = builder.GetBuilder().CreateFNeg(ov), assign = false;

    if (value)
    {
        if (assign)
        {
            operand->Store(value);
            if (OpRight)
                return RValue::Create(builder, Type, ov);
            return operand;
        }
        return RValue::Create(builder, Type, value);
    }

    Error(Where, "undefined unary operator {}{}", Op, Type);
}

std::ostream& NJS::UnaryExpr::Print(std::ostream& os)
{
    return Operand->Print(os << (OpRight ? "" : Op)) << (OpRight ? Op : "");
}
