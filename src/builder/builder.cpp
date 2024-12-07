#include <ranges>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::Builder::Builder(TypeContext& ctx, llvm::LLVMContext& context, const std::string& module_id, const bool is_main)
    : m_Ctx(ctx), m_LLVMContext(context), m_ModuleID(module_id)
{
    m_LLVMModule = std::make_unique<llvm::Module>(module_id, m_LLVMContext);
    m_LLVMBuilder = std::make_unique<llvm::IRBuilder<>>(m_LLVMContext);

    Push(m_ModuleID);

    const auto process = DefVar("process") = CreateGlobal(
        "process",
        m_Ctx.GetObjectType({
            {"args", m_Ctx.GetVectorType(m_Ctx.GetStringType())},
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

        const auto argc = function->getArg(0);
        const auto argv = function->getArg(1);

        const auto args = CreateMember(process, "args");
        const auto argc_i64 = GetBuilder().CreateIntCast(argc, GetBuilder().getInt64Ty(), false);
        args->Store(CreateVector(m_Ctx.GetStringType(), argv, argc_i64));
    }
    else
    {
        const auto type = llvm::FunctionType::get(GetBuilder().getInt32Ty(), false);
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
    GetBuilder().CreateRet(GetBuilder().getInt32(0));
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