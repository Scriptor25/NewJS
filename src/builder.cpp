#include <ranges>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

#include "NJS/Context.hpp"

bool NJS::StackFrame::contains(const std::string& name) const
{
    return Values.contains(name);
}

NJS::ValuePtr NJS::StackFrame::operator[](const std::string& name) const
{
    return Values.at(name);
}

NJS::ValuePtr& NJS::StackFrame::operator[](const std::string& name)
{
    return Values[name];
}

std::string NJS::StackFrame::ValueName(const std::string& name) const
{
    if (name.empty()) return ParentName;
    if (ParentName.empty()) return name;
    return ParentName + '.' + name;
}

NJS::Builder::Builder(Context& ctx, llvm::LLVMContext& context, const std::string& module_id, const bool is_main)
    : m_Ctx(ctx), m_LLVMContext(context), m_ModuleID(module_id)
{
    m_LLVMModule = std::make_unique<llvm::Module>(module_id, m_LLVMContext);
    m_LLVMBuilder = std::make_unique<llvm::IRBuilder<>>(m_LLVMContext);

    Push(m_ModuleID);

    {
        const auto type = llvm::FunctionType::get(GetBuilder().getInt32Ty(), false);
        const auto function = llvm::Function::Create(
            type,
            llvm::GlobalValue::ExternalLinkage,
            is_main ? "main" : GetName("main"),
            GetModule());
        GetBuilder().SetInsertPoint(llvm::BasicBlock::Create(GetContext(), "entry", function));
    }
}

void NJS::Builder::Close()
{
    GetBuilder().CreateRet(GetBuilder().getInt32(0));
    Pop();
}

NJS::Context& NJS::Builder::GetCtx() const
{
    return m_Ctx;
}

std::unique_ptr<llvm::Module>&& NJS::Builder::MoveModule()
{
    return std::move(m_LLVMModule);
}

llvm::LLVMContext& NJS::Builder::GetContext() const
{
    return m_LLVMContext;
}

llvm::Module& NJS::Builder::GetModule() const
{
    return *m_LLVMModule;
}

llvm::IRBuilder<>& NJS::Builder::GetBuilder() const
{
    return *m_LLVMBuilder;
}

llvm::Value* NJS::Builder::CreateMalloc(const size_t size) const
{
    const auto type = llvm::FunctionType::get(
        GetBuilder().getPtrTy(),
        {GetBuilder().getInt64Ty()},
        false);
    const auto callee = GetModule().getOrInsertFunction("malloc", type);
    return GetBuilder().CreateCall(callee, {GetBuilder().getInt64(size)});
}

llvm::Value* NJS::Builder::CreateRealloc(llvm::Value* ptr, llvm::Value* size) const
{
    const auto type = llvm::FunctionType::get(
        GetBuilder().getPtrTy(),
        {GetBuilder().getPtrTy(), GetBuilder().getInt64Ty()},
        false);
    const auto callee = GetModule().getOrInsertFunction("realloc", type);
    return GetBuilder().CreateCall(callee, {ptr, size});
}

llvm::Value* NJS::Builder::CreateMemcpy(llvm::Value* dst, llvm::Value* src, llvm::Value* count) const
{
    const auto type = llvm::FunctionType::get(
        GetBuilder().getPtrTy(),
        {GetBuilder().getPtrTy(), GetBuilder().getPtrTy(), GetBuilder().getInt64Ty()},
        false);
    const auto callee = GetModule().getOrInsertFunction("memcpy", type);
    return GetBuilder().CreateCall(callee, {dst, src, count});
}

llvm::Value* NJS::Builder::CreateStrlen(llvm::Value* str) const
{
    const auto type = llvm::FunctionType::get(
        GetBuilder().getInt64Ty(),
        {GetBuilder().getPtrTy()},
        false);
    const auto callee = GetModule().getOrInsertFunction("strlen", type);
    return GetBuilder().CreateCall(callee, {str});
}

llvm::Value* NJS::Builder::CreateAlloca(llvm::Type* type, const size_t size) const
{
    const auto bkp = GetBuilder().GetInsertBlock();
    GetBuilder().SetInsertPointPastAllocas(bkp->getParent());
    const auto ptr = GetBuilder().CreateAlloca(type, size ? GetBuilder().getInt64(size) : nullptr);
    GetBuilder().SetInsertPoint(bkp);
    return ptr;
}

NJS::ValuePtr NJS::Builder::CreateAlloca(const TypePtr& type, const size_t size)
{
    const auto ptr = CreateAlloca(type->GenLLVM(*this), size);
    return LValue::Create(*this, type, ptr);
}

llvm::Value* NJS::Builder::CreateEmpty(const TypePtr& type)
{
    const auto ty = type->GenLLVM(*this);

    if (type->IsPrimitive(Primitive_String))
        return ConstStringExpr::GetString(*this, {});

    if (type->IsPrimitive())
        return llvm::Constant::getNullValue(ty);

    if (type->IsArray() || type->IsTuple() || type->IsObject())
    {
        llvm::Value* value = llvm::Constant::getNullValue(ty);
        for (size_t i = 0; i < type->NumElements(); ++i)
            value = GetBuilder().CreateInsertValue(value, CreateEmpty(type->Element(i)), i);
        return value;
    }

    if (type->IsVector())
    {
        const auto vec_ty = VectorType::GenVecLLVM(*this);
        const auto ptr = CreateMalloc(16);

        const auto gep_ptr = GetBuilder().CreateStructGEP(vec_ty, ptr, 0);
        GetBuilder().CreateStore(llvm::Constant::getNullValue(GetBuilder().getPtrTy()), gep_ptr);

        const auto gep_size = GetBuilder().CreateStructGEP(vec_ty, ptr, 1);
        GetBuilder().CreateStore(llvm::Constant::getNullValue(GetBuilder().getInt64Ty()), gep_size);

        return ptr;
    }

    Error("no empty value for type {}", type);
}

NJS::ValuePtr NJS::Builder::CreateSubscript(const ValuePtr& array, llvm::Value* index)
{
    const auto type = array->GetType();

    if (!(type->IsPrimitive(Primitive_String) || type->IsArray() || type->IsTuple() || type->IsVector()))
        Error(
            "invalid subscript: first operand type must be indexable, like string, array, tuple or vector, but is {}",
            type);

    const auto size_ty = GetBuilder().getInt64Ty();

    if (type->IsVector())
    {
        const auto ptr = array->Load();
        const auto vec_ty = VectorType::GenVecLLVM(*this);

        const auto gep_ptr = GetBuilder().CreateStructGEP(vec_ty, ptr, 0);
        const auto vec_ptr = GetBuilder().CreateLoad(GetBuilder().getPtrTy(), gep_ptr);

        const auto gep_size = GetBuilder().CreateStructGEP(vec_ty, ptr, 1);
        const auto vec_size = GetBuilder().CreateLoad(size_ty, gep_size);

        const auto zero = llvm::Constant::getNullValue(size_ty);
        const auto cmp_lower = GetBuilder().CreateICmpSGE(index, zero);
        const auto cmp_upper = GetBuilder().CreateICmpSLT(index, vec_size);
        const auto condition = GetBuilder().CreateAnd(cmp_lower, cmp_upper);

        // TODO: do something with the out of bounds check
        (void)condition;

        const auto element_type = type->Element();
        const auto el_ty = element_type->GenLLVM(*this);
        const auto gep = GetBuilder().CreateGEP(el_ty, vec_ptr, index);
        return LValue::Create(*this, element_type, gep);
    }

    if (array->GetType()->IsPrimitive(Primitive_String))
    {
        const auto ptr = array->Load();
        const auto gep = GetBuilder().CreateGEP(GetBuilder().getInt8Ty(), ptr, index);
        const auto chr = GetBuilder().CreateLoad(GetBuilder().getInt8Ty(), gep);
        return RValue::Create(*this, GetCtx().GetCharType(), chr);
    }

    if (array->IsL())
    {
        const auto ty = type->GenLLVM(*this);
        const auto ptr = array->GetPtr();
        const auto zero = llvm::Constant::getNullValue(size_ty);
        const auto gep = GetBuilder().CreateInBoundsGEP(ty, ptr, {zero, index});
        return LValue::Create(*this, type->Element(), gep);
    }

    const auto const_idx = llvm::dyn_cast<llvm::ConstantInt>(index);
    const auto value = GetBuilder().CreateExtractValue(array->Load(), *const_idx->getValue().getRawData());
    return RValue::Create(*this, type->Element(), value);
}

void NJS::Builder::GetFormat(llvm::FunctionCallee& ref) const
{
    std::vector<llvm::Type*> param_types(2);
    param_types[0] = GetBuilder().getPtrTy();
    param_types[1] = GetBuilder().getInt64Ty();
    const auto type = llvm::FunctionType::get(GetBuilder().getVoidTy(), param_types, true);
    ref = GetModule().getOrInsertFunction("format", type);
}

void NJS::Builder::Push(const std::string& name)
{
    const auto frame_name = m_Stack.empty() ? name : m_Stack.back().ValueName(name);
    m_Stack.emplace_back(frame_name);
}

void NJS::Builder::Pop()
{
    m_Stack.pop_back();
}

std::string NJS::Builder::GetName(const std::string& name) const
{
    return m_Stack.back().ValueName(name);
}

NJS::ValuePtr& NJS::Builder::DefVar(const std::string& name)
{
    auto& stack = m_Stack.back();
    if (stack.contains(name)) Error("cannot redefine symbol '{}'", name);
    return stack[name];
}

NJS::ValuePtr& NJS::Builder::GetVar(const std::string& name)
{
    for (auto& stack : std::ranges::reverse_view(m_Stack))
        if (stack.contains(name)) return stack[name];
    Error("undefined symbol '{}'", name);
}
