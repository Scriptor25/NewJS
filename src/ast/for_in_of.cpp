#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Context.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::ForInOfStmt::ForInOfStmt(VariableStmt init, const bool of, ExprPtr value, StmtPtr body)
    : Init(std::move(init)), Of(of), Value(std::move(value)), Body(std::move(body))
{
}

NJS::ValuePtr NJS::ForInOfStmt::GenLLVM(Builder& builder)
{
    const auto parent = builder.GetBuilder().GetInsertBlock()->getParent();
    const auto head = llvm::BasicBlock::Create(builder.GetContext(), "head", parent);
    const auto loop = llvm::BasicBlock::Create(builder.GetContext(), "loop", parent);
    const auto end = llvm::BasicBlock::Create(builder.GetContext(), "end", parent);

    builder.Push();

    const auto value = Value->GenLLVM(builder);
    const auto i_ptr = builder.CreateAlloca(builder.GetBuilder().getInt64Ty());
    builder.GetBuilder().CreateStore(builder.GetBuilder().getInt64(0), i_ptr);
    builder.GetBuilder().CreateBr(head);

    builder.GetBuilder().SetInsertPoint(head);
    const auto i_int = builder.GetBuilder().CreateLoad(builder.GetBuilder().getInt64Ty(), i_ptr);
    const auto i_inc = builder.GetBuilder().CreateAdd(i_int, builder.GetBuilder().getInt64(1));
    builder.GetBuilder().CreateStore(i_inc, i_ptr);
    ValuePtr init;
    if (Of)
    {
        init = builder.CreateSubscript(value, i_int);
    }
    else
    {
        const auto i_fp = builder.GetBuilder().CreateUIToFP(i_int, builder.GetBuilder().getDoubleTy());
        init = RValue::Create(builder, builder.GetCtx().GetNumberType(), i_fp);
    }
    Init.GenLLVM(builder, init);
    llvm::Value* count;
    if (value->GetType()->IsPrimitive(Primitive_String))
        count = builder.CreateStrlen(value->Load());
    else count = builder.GetBuilder().getInt64(value->GetType()->NumElements());
    const auto condition = builder.GetBuilder().CreateICmpULT(i_int, count);
    builder.GetBuilder().CreateCondBr(condition, loop, end);

    builder.GetBuilder().SetInsertPoint(loop);
    Body->GenLLVM(builder);
    builder.GetBuilder().CreateBr(head);

    builder.GetBuilder().SetInsertPoint(end);

    builder.Pop();
    return {};
}

std::ostream& NJS::ForInOfStmt::Print(std::ostream& os)
{
    return Body->Print(Value->Print(Init.Print(os << "for (") << ' ' << (Of ? "of" : "in") << ' ') << ") ");
}
