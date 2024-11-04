#include <ranges>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

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

NJS::Builder::Builder(Context& ctx, const std::string& module_id)
    : m_Ctx(ctx)
{
    m_LLVMContext = std::make_unique<llvm::LLVMContext>();
    m_LLVMModule = std::make_unique<llvm::Module>(module_id, *m_LLVMContext);
    m_LLVMBuilder = std::make_unique<llvm::IRBuilder<>>(*m_LLVMContext);

    {
        const auto type = llvm::FunctionType::get(LLVMBuilder().getInt32Ty(), false);
        const auto function = llvm::Function::Create(type, llvm::GlobalValue::ExternalLinkage, "main", LLVMModule());
        LLVMBuilder().SetInsertPoint(llvm::BasicBlock::Create(LLVMContext(), "entry", function));
    }

    Push();
}

void NJS::Builder::Close() const
{
    LLVMBuilder().CreateRet(LLVMBuilder().getInt32(0));
    LLVMModule().print(llvm::outs(), nullptr);
}

NJS::Context& NJS::Builder::Ctx() const
{
    return m_Ctx;
}

llvm::LLVMContext& NJS::Builder::LLVMContext() const
{
    return *m_LLVMContext;
}

llvm::Module& NJS::Builder::LLVMModule() const
{
    return *m_LLVMModule;
}

llvm::IRBuilder<>& NJS::Builder::LLVMBuilder() const
{
    return *m_LLVMBuilder;
}

NJS::ValuePtr NJS::Builder::CreateAlloca(const TypePtr& type)
{
    const auto ptr = CreateAlloca(type->GenLLVM(*this), 0);
    return LValue::Create(*this, type, ptr);
}

llvm::Value* NJS::Builder::CreateAlloca(llvm::Type* type, const size_t size) const
{
    const auto bkp = LLVMBuilder().GetInsertBlock();
    LLVMBuilder().SetInsertPointPastAllocas(bkp->getParent());
    const auto ptr = LLVMBuilder().CreateAlloca(type, size ? LLVMBuilder().getInt64(size) : nullptr);
    LLVMBuilder().SetInsertPoint(bkp);
    return ptr;
}

NJS::ValuePtr NJS::Builder::CreateGlobal(
    const std::string& name,
    const TypePtr& type,
    const bool is_const,
    const ValuePtr& init)
{
    const auto llvm_type = type->GenLLVM(*this);
    const auto init_value = init ? init->Load() : nullptr;

    auto const_init = init_value ? llvm::dyn_cast<llvm::Constant>(init_value) : nullptr;
    const auto not_init = init_value && !const_init;

    if (not_init) const_init = llvm::Constant::getNullValue(llvm_type);

    const auto gv = new llvm::GlobalVariable(
        LLVMModule(),
        llvm_type,
        is_const,
        llvm::GlobalValue::InternalLinkage,
        const_init,
        name);

    if (not_init) LLVMBuilder().CreateStore(init_value, gv);
    return LValue::Create(*this, type, gv);
}

void NJS::Builder::GetFormat(llvm::FunctionCallee& ref) const
{
    std::vector<llvm::Type*> param_types(3);
    param_types[0] = LLVMBuilder().getPtrTy();
    param_types[1] = LLVMBuilder().getInt64Ty();
    param_types[2] = LLVMBuilder().getInt64Ty();
    const auto type = llvm::FunctionType::get(LLVMBuilder().getVoidTy(), param_types, true);
    ref = LLVMModule().getOrInsertFunction("format", type);
}

void NJS::Builder::GetMalloc(llvm::FunctionCallee& ref) const
{
    std::vector<llvm::Type*> param_types(1);
    param_types[0] = LLVMBuilder().getInt64Ty();
    const auto type = llvm::FunctionType::get(LLVMBuilder().getPtrTy(), param_types, false);
    ref = LLVMModule().getOrInsertFunction("malloc", type);
}

void NJS::Builder::Push(const std::string& name)
{
    const auto frame_name = m_Stack.empty()
                                ? name
                                : m_Stack.back().ParentName.empty()
                                ? name
                                : name.empty()
                                ? m_Stack.back().ParentName
                                : m_Stack.back().ParentName + '.' + name;
    m_Stack.emplace_back(frame_name);
}

void NJS::Builder::Pop()
{
    m_Stack.pop_back();
}

std::string NJS::Builder::ValueName(const std::string& name) const
{
    return m_Stack.back().ParentName.empty() ? name : m_Stack.back().ParentName + '.' + name;
}

NJS::ValuePtr& NJS::Builder::CreateVar(const std::string& name)
{
    auto& stack = m_Stack.back();
    if (stack.contains(name)) Error("cannot redefine symbol");
    return stack[name];
}

NJS::ValuePtr& NJS::Builder::GetVar(const std::string& name)
{
    for (auto& stack : std::ranges::reverse_view(m_Stack))
        if (stack.contains(name)) return stack[name];
    Error("undefined symbol");
}
