#include <ranges>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::Builder::Builder(TypeContext& ctx, llvm::LLVMContext& context, const std::string& module_id, const bool is_main)
    : m_Ctx(ctx), m_LLVMContext(context), m_ModuleID(module_id), m_IsMain(is_main)
{
    m_LLVMModule = std::make_unique<llvm::Module>(module_id, m_LLVMContext);
    m_LLVMBuilder = std::make_unique<llvm::IRBuilder<>>(m_LLVMContext);

    Push(m_ModuleID);

    const auto process = DefVar("process") = CreateGlobal(
        "process",
        m_Ctx.GetStructType({
            {"argc", m_Ctx.GetIntType(32, true)},
            {"argv", m_Ctx.GetPointerType(m_Ctx.GetPointerType(m_Ctx.GetIntType(8, true)))}
        }), is_main);

    llvm::Function* function;
    if (is_main)
    {
        const auto type = llvm::FunctionType::get(
            GetBuilder().getInt32Ty(),
            {GetBuilder().getInt32Ty(), GetBuilder().getPtrTy()},
            false);
        function = llvm::Function::Create(
            type,
            llvm::GlobalValue::ExternalLinkage,
            "main",
            GetModule());
        GetBuilder().SetInsertPoint(llvm::BasicBlock::Create(GetContext(), "entry", function));

        CreateMember(process, "argc")->Store(function->getArg(0));
        CreateMember(process, "argv")->Store(function->getArg(1));
    }
    else
    {
        const auto type = llvm::FunctionType::get(GetBuilder().getVoidTy(), false);
        function = llvm::Function::Create(
            type,
            llvm::GlobalValue::ExternalLinkage,
            GetName("main"),
            GetModule());
        GetBuilder().SetInsertPoint(llvm::BasicBlock::Create(GetContext(), "entry", function));
    }
}

void NJS::Builder::Close()
{
    if (m_IsMain)
        GetBuilder().CreateRet(GetBuilder().getInt32(0));
    else GetBuilder().CreateRetVoid();
    Pop();
}

NJS::TypeContext& NJS::Builder::GetCtx() const
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
