#include <ranges>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/SourceLocation.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::Builder::Builder(
    TypeContext &ctx,
    llvm::LLVMContext &context,
    const std::string_view &module_id,
    const bool is_main)
    : m_TypeContext(ctx),
      m_LLVMContext(context),
      m_ModuleID(module_id),
      m_IsMain(is_main)
{
    m_LLVMModule = std::make_unique<llvm::Module>(module_id, m_LLVMContext);
    m_LLVMBuilder = std::make_unique<llvm::IRBuilder<>>(m_LLVMContext);

    StackPush(m_ModuleID);

    auto &process = DefineVariable({}, "process");
    process = CreateGlobal(
        {},
        "process",
        m_TypeContext.GetStructType(
            {
                {"argc", m_TypeContext.GetIntegerType(32, true)},
                {"argv", m_TypeContext.GetPointerType(m_TypeContext.GetStringType())}
            }),
        is_main);

    llvm::Function *function;
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
        function->getArg(0)->setName("argc");
        function->getArg(1)->setName("argv");
        GetBuilder().SetInsertPoint(llvm::BasicBlock::Create(GetContext(), "entry", function));
        CreateMember({}, process, "argc")->Store({}, function->getArg(0), true);
        CreateMember({}, process, "argv")->Store({}, function->getArg(1), true);
    }
    else
    {
        const auto type = llvm::FunctionType::get(GetBuilder().getVoidTy(), false);
        function = llvm::Function::Create(
            type,
            llvm::GlobalValue::ExternalLinkage,
            GetName(true, "main"),
            GetModule());
        GetBuilder().SetInsertPoint(llvm::BasicBlock::Create(GetContext(), "entry", function));
    }
}

void NJS::Builder::Close()
{
    if (m_IsMain)
        GetBuilder().CreateRet(GetBuilder().getInt32(0));
    else
        GetBuilder().CreateRetVoid();
    StackPop();
}

NJS::TypeContext &NJS::Builder::GetTypeContext() const
{
    return m_TypeContext;
}

std::unique_ptr<llvm::Module> &&NJS::Builder::MoveModule()
{
    return std::move(m_LLVMModule);
}

llvm::LLVMContext &NJS::Builder::GetContext() const
{
    return m_LLVMContext;
}

llvm::Module &NJS::Builder::GetModule() const
{
    return *m_LLVMModule;
}

llvm::IRBuilder<> &NJS::Builder::GetBuilder() const
{
    return *m_LLVMBuilder;
}

void NJS::Builder::GetFormat(llvm::FunctionCallee &callee) const
{
    std::vector<llvm::Type *> param_types(2);
    param_types[0] = GetBuilder().getPtrTy();
    param_types[1] = GetBuilder().getInt64Ty();
    const auto type = llvm::FunctionType::get(GetBuilder().getVoidTy(), param_types, true);
    callee = GetModule().getOrInsertFunction("format", type);
}

void NJS::Builder::StackPush(const std::string_view &name, const TypePtr &result_type)
{
    const auto frame_name = m_Stack.empty()
                                ? std::string(name)
                                : m_Stack.back().GetChildName(name);
    const auto frame_result_type = result_type
                                       ? result_type
                                       : m_Stack.empty()
                                             ? nullptr
                                             : m_Stack.back().ResultType;
    m_Stack.emplace_back(frame_name, frame_result_type);
}

void NJS::Builder::StackPop()
{
    m_Stack.pop_back();
}

std::string NJS::Builder::GetName(const bool absolute, const std::string_view &name) const
{
    if (absolute)
        return m_ModuleID + '.' + std::string(name);
    return m_Stack.back().GetChildName(name);
}

void NJS::Builder::DefineOperator(
    const std::string_view &sym,
    const TypePtr &val,
    const TypePtr &result,
    llvm::Value *callee)
{
    m_UnaryOperators[std::string(sym)][val] = {result, val, callee};
}

void NJS::Builder::DefineOperator(
    const std::string_view &sym,
    const TypePtr &lhs,
    const TypePtr &rhs,
    const TypePtr &result,
    llvm::Value *callee)
{
    m_BinaryOperators[std::string(sym)][lhs][rhs] = {result, lhs, rhs, callee};
}

NJS::OperatorInfo<1> NJS::Builder::GetOperator(const std::string_view &sym, const TypePtr &val)
{
    return m_UnaryOperators[std::string(sym)][val];
}

NJS::OperatorInfo<2> NJS::Builder::GetOperator(const std::string_view &sym, const TypePtr &lhs, const TypePtr &rhs)
{
    return m_BinaryOperators[std::string(sym)][lhs][rhs];
}

NJS::OperatorInfo<2> NJS::Builder::FindOperator(
    const std::string_view &sym,
    const TypePtr &lhs,
    const bool lhs_is_ref_able,
    const TypePtr &rhs,
    const bool rhs_is_ref_able)
{
    const auto lhs_ref = lhs_is_ref_able ? GetTypeContext().GetReferenceType(lhs) : lhs;
    const auto rhs_ref = rhs_is_ref_able ? GetTypeContext().GetReferenceType(rhs) : rhs;
    if (auto o = GetOperator(sym, lhs_ref, rhs_ref); o.Callee)
        return o;
    if (auto o = GetOperator(sym, lhs, rhs_ref); o.Callee)
        return o;
    if (auto o = GetOperator(sym, lhs_ref, rhs); o.Callee)
        return o;
    if (auto o = GetOperator(sym, lhs, rhs); o.Callee)
        return o;
    return {};
}

NJS::ValuePtr &NJS::Builder::DefineVariable(const SourceLocation &where, const std::string_view &name)
{
    auto &stack = m_Stack.back();
    if (stack.Contains(name))
        Error(where, "cannot redefine symbol '{}'", name);
    return stack[name];
}

NJS::ValuePtr &NJS::Builder::GetVariable(const SourceLocation &where, const std::string_view &name)
{
    for (auto &stack: std::ranges::reverse_view(m_Stack))
        if (stack.Contains(name))
            return stack[name];
    Error(where, "undefined symbol '{}'", name);
}

NJS::TypePtr &NJS::Builder::CurrentFunctionResultType()
{
    return m_Stack.back().ResultType;
}
