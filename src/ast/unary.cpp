#include <llvm/IR/Value.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Context.hpp>
#include <NJS/Error.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Value.hpp>

NJS::UnaryExpr::UnaryExpr(std::string op, const bool op_right, ExprPtr operand)
    : Op(std::move(op)), OpRight(op_right), Operand(std::move(operand))
{
}

NJS::ValuePtr NJS::UnaryExpr::GenLLVM(Builder& builder)
{
    const auto operand = Operand->GenLLVM(builder);

    const auto ov = operand->Load();
    const auto one = llvm::ConstantFP::get(builder.LLVMBuilder().getDoubleTy(), 1.0);

    llvm::Value* value{};
    bool assign = false;
    if (Op == "++") value = builder.LLVMBuilder().CreateFAdd(ov, one), assign = true;
    else if (Op == "--") value = builder.LLVMBuilder().CreateFSub(ov, one), assign = true;

    if (value)
    {
        if (assign)
        {
            operand->Store(value);
            return operand;
        }
        return RValue::Create(builder, builder.Ctx().GetNumberType(), value);
    }

    Error("undefined unary operator");
}

std::ostream& NJS::UnaryExpr::Print(std::ostream& os)
{
    return Operand->Print(os << (OpRight ? "" : Op)) << (OpRight ? Op : "");
}
