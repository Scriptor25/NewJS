#include <ranges>
#include <utility>
#include <llvm/IR/Verifier.h>
#include <newjs/builder.hpp>
#include <newjs/error.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

NJS::Builder::Builder(
    TypeContext &type_context,
    llvm::LLVMContext &llvm_context,
    const std::string &module_id,
    const std::string &source_filename,
    const bool is_main)
    : m_ModuleID(module_id),
      m_IsMain(is_main),
      m_TypeContext(type_context),
      m_LLVMContext(llvm_context),
      m_PassManager(llvm_context)
{
    m_LLVMBuilder = std::make_unique<llvm::IRBuilder<>>(m_LLVMContext);
    m_LLVMModule = std::make_unique<llvm::Module>(module_id, m_LLVMContext);
    m_LLVMModule->setSourceFileName(source_filename);

    if (is_main)
        StackPush(m_ModuleID, ReferenceInfo(m_TypeContext.GetIntegerType(32, true)));
    else
        StackPush(m_ModuleID, ReferenceInfo(m_TypeContext.GetVoidType()));

    const auto process = CreateGlobal(
        "process",
        m_TypeContext.GetStructType(
            {
                {"argc", m_TypeContext.GetIntegerType(32, true)},
                {"argv", m_TypeContext.GetPointerType(m_TypeContext.GetStringType(), false)},
            },
            {}),
        false,
        is_main);
    DefineVariable("process", true) = process;

    if (is_main)
    {
        const auto type = llvm::FunctionType::get(
            GetBuilder().getInt32Ty(),
            {GetBuilder().getInt32Ty(), GetBuilder().getPtrTy()},
            false);
        m_Function = llvm::Function::Create(
            type,
            llvm::GlobalValue::ExternalLinkage,
            "main",
            GetModule());
        m_Function->getArg(0)->setName("argc");
        m_Function->getArg(1)->setName("argv");
        GetBuilder().SetInsertPoint(llvm::BasicBlock::Create(GetContext(), "entry", m_Function));
        CreateMember(process, "argc").Value->Store(m_Function->getArg(0));
        CreateMember(process, "argv").Value->Store(m_Function->getArg(1));
        return;
    }

    const auto type = llvm::FunctionType::get(GetBuilder().getVoidTy(), false);
    m_Function = llvm::Function::Create(
        type,
        llvm::GlobalValue::ExternalLinkage,
        GetName(true, "main"),
        GetModule());
    GetBuilder().SetInsertPoint(llvm::BasicBlock::Create(GetContext(), "entry", m_Function));
}

void NJS::Builder::Close()
{
    if (!GetBuilder().GetInsertBlock()->getTerminator())
    {
        if (m_IsMain)
            GetBuilder().CreateRet(GetBuilder().getInt32(0));
        else
            GetBuilder().CreateRetVoid();
    }
    StackPop();

    std::vector<llvm::BasicBlock *> deletable;
    for (auto &block: *m_Function)
    {
        if (&block != &m_Function->getEntryBlock() && block.hasNPredecessors(0) && block.empty())
        {
            deletable.emplace_back(&block);
            continue;
        }

        if (block.getTerminator())
            continue;

        if (m_Function->getReturnType()->isVoidTy())
        {
            GetBuilder().SetInsertPoint(&block);
            GetBuilder().CreateRetVoid();
            continue;
        }

        m_Function->print(llvm::errs());
        Error("not all code paths return in module main function (module {})", m_ModuleID);
    }

    for (const auto block: deletable)
        block->eraseFromParent();

    if (verifyFunction(*m_Function, &llvm::errs()))
    {
        m_LLVMModule->print(llvm::errs(), nullptr);
        Error("failed to verify module main function (module {})", m_ModuleID);
    }

    Optimize(m_Function);
}

const std::string &NJS::Builder::GetModuleID() const
{
    return m_ModuleID;
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

void NJS::Builder::Optimize(llvm::Function *function) const
{
    m_PassManager.FPM().run(*function, m_PassManager.FAM());
}

void NJS::Builder::GetFormat(llvm::FunctionCallee &callee) const
{
    const auto type = llvm::FunctionType::get(
        GetBuilder().getPtrTy(),
        {GetBuilder().getInt32Ty()},
        true);

    if (const auto function = GetModule().getFunction("format"))
    {
        callee = llvm::FunctionCallee(type, function);
        return;
    }

    const auto function = llvm::Function::Create(type, llvm::Function::ExternalLinkage, "format", GetModule());
    callee = llvm::FunctionCallee(type, function);
}

void NJS::Builder::StackPush(
    const std::string &name,
    const ReferenceInfo &result,
    llvm::BasicBlock *head_block,
    llvm::BasicBlock *tail_block)
{
    const auto is_function_entry = static_cast<bool>(result.Type);
    std::map<std::string, std::pair<bool, ValuePtr>> values;

    if (m_Stack.empty())
    {
        m_Stack.emplace_back(
            is_function_entry,
            name,
            result,
            head_block,
            tail_block,
            values);
        return;
    }

    const auto frame_name = m_Stack.back().GetChildName(name);

    if (is_function_entry)
    {
        m_Stack.emplace_back(
            is_function_entry,
            frame_name,
            result,
            head_block,
            tail_block,
            values);
        return;
    }

    const auto frame_result = m_Stack.back().Result;
    const auto frame_head_block = head_block ? head_block : m_Stack.back().HeadBlock;
    const auto frame_tail_block = tail_block ? tail_block : m_Stack.back().TailBlock;

    m_Stack.emplace_back(
        is_function_entry,
        frame_name,
        frame_result,
        frame_head_block,
        frame_tail_block,
        values);
}

void NJS::Builder::StackPop()
{
    m_Stack.pop_back();
}

std::string NJS::Builder::GetName(const bool absolute, const std::string &name) const
{
    if (absolute)
        return m_ModuleID + '.' + name;
    return m_Stack.back().GetChildName(name);
}

void NJS::Builder::DefineOperator(
    const ReferenceInfo &callee_info,
    const std::vector<ReferenceInfo> &parameter_infos,
    const bool is_var_arg,
    const ReferenceInfo &result_info,
    llvm::Value *callee)
{
    m_CallOperatorMap[callee_info] = {callee_info, parameter_infos, is_var_arg, result_info, callee};
}

void NJS::Builder::DefineOperator(
    const std::string &name,
    const bool prefix,
    const ReferenceInfo &value,
    const ReferenceInfo &result,
    llvm::Value *callee)
{
    m_UnaryOperatorMap[name][prefix][value] = {result, value, callee};
}

void NJS::Builder::DefineOperator(
    const std::string &name,
    const ReferenceInfo &left,
    const ReferenceInfo &right,
    const ReferenceInfo &result,
    llvm::Value *callee)
{
    m_BinaryOperatorMap[name][left][right] = {result, left, right, callee};
}

NJS::CallOperatorInfo NJS::Builder::GetOperator(const ReferenceInfo &callee_info)
{
    return m_CallOperatorMap[callee_info];
}

NJS::OperatorInfo<1> NJS::Builder::GetOperator(
    const std::string &name,
    const bool prefix,
    const ReferenceInfo &value) const
{
    if (!m_UnaryOperatorMap.contains(name))
        return {};
    auto &for_name = m_UnaryOperatorMap.at(name);
    if (!for_name.contains(prefix))
        return {};
    auto &for_prefix = for_name.at(prefix);
    if (!for_prefix.contains(value))
        return {};
    return for_prefix.at(value);
}

NJS::OperatorInfo<2> NJS::Builder::GetOperator(
    const std::string &name,
    const ReferenceInfo &left,
    const ReferenceInfo &right) const
{
    if (!m_BinaryOperatorMap.contains(name))
        return {};
    auto &for_name = m_BinaryOperatorMap.at(name);
    if (!for_name.contains(left))
        return {};
    auto &for_left = for_name.at(left);
    if (!for_left.contains(right))
        return {};
    return for_left.at(right);
}

NJS::CallOperatorInfo NJS::Builder::FindOperator(const ValuePtr &callee)
{
    const auto v_ty = callee->GetType();
    const auto v_const = callee->IsConst();
    const auto v_ref = callee->IsLValue();

    if (auto o = GetOperator({v_ty, v_const, v_ref}); o.Callee)
        return o;
    if (auto o = GetOperator({v_ty, true, v_ref}); o.Callee)
        return o;
    if (auto o = GetOperator({v_ty, true, true}); o.Callee)
        return o;
    if (auto o = GetOperator({v_ty, false, false}); o.Callee)
        return o;

    return {};
}

NJS::OperatorInfo<1> NJS::Builder::FindOperator(
    const std::string &name,
    const bool prefix,
    const ValuePtr &value) const
{
    const auto v_ty = value->GetType();
    const auto v_const = value->IsConst();
    const auto v_ref = value->IsLValue();

    if (auto o = GetOperator(name, prefix, {v_ty, v_const, v_ref}); o.Callee)
        return o;
    if (auto o = GetOperator(name, prefix, {v_ty, true, v_ref}); o.Callee)
        return o;
    if (auto o = GetOperator(name, prefix, {v_ty, true, true}); o.Callee)
        return o;
    if (auto o = GetOperator(name, prefix, {v_ty, false, false}); o.Callee)
        return o;

    return {};
}

NJS::OperatorInfo<2> NJS::Builder::FindOperator(
    const std::string &name,
    const ValuePtr &left,
    const ValuePtr &right) const
{
    const auto l_ty = left->GetType();
    const auto r_ty = right->GetType();
    const auto l_const = left->IsConst();
    const auto r_const = right->IsConst();
    const auto l_ref = left->IsLValue();
    const auto r_ref = right->IsLValue();

    if (auto o = GetOperator(name, {l_ty, l_const, l_ref}, {r_ty, r_const, r_ref}); o.Callee)
        return o;
    if (auto o = GetOperator(name, {l_ty, l_const, l_ref}, {r_ty, true, r_ref}); o.Callee)
        return o;
    if (auto o = GetOperator(name, {l_ty, l_const, l_ref}, {r_ty, true, true}); o.Callee)
        return o;
    if (auto o = GetOperator(name, {l_ty, l_const, l_ref}, {r_ty, false, false}); o.Callee)
        return o;
    if (auto o = GetOperator(name, {l_ty, true, l_ref}, {r_ty, r_const, r_ref}); o.Callee)
        return o;
    if (auto o = GetOperator(name, {l_ty, true, l_ref}, {r_ty, true, r_ref}); o.Callee)
        return o;
    if (auto o = GetOperator(name, {l_ty, true, l_ref}, {r_ty, true, true}); o.Callee)
        return o;
    if (auto o = GetOperator(name, {l_ty, true, l_ref}, {r_ty, false, false}); o.Callee)
        return o;
    if (auto o = GetOperator(name, {l_ty, true, true}, {r_ty, r_const, r_ref}); o.Callee)
        return o;
    if (auto o = GetOperator(name, {l_ty, true, true}, {r_ty, true, r_ref}); o.Callee)
        return o;
    if (auto o = GetOperator(name, {l_ty, true, true}, {r_ty, true, true}); o.Callee)
        return o;
    if (auto o = GetOperator(name, {l_ty, true, true}, {r_ty, false, false}); o.Callee)
        return o;
    if (auto o = GetOperator(name, {l_ty, false, false}, {r_ty, r_const, r_ref}); o.Callee)
        return o;
    if (auto o = GetOperator(name, {l_ty, false, false}, {r_ty, true, r_ref}); o.Callee)
        return o;
    if (auto o = GetOperator(name, {l_ty, false, false}, {r_ty, true, true}); o.Callee)
        return o;
    if (auto o = GetOperator(name, {l_ty, false, false}, {r_ty, false, false}); o.Callee)
        return o;

    return {};
}

NJS::ValuePtr &NJS::Builder::DefineVariable(const std::string &name, const bool is_global)
{
    if (m_Stack.back().Contains(name))
        Error("cannot redefine symbol '{}'", name);

    auto &[is_global_, value_] = m_Stack.back()[name];
    is_global_ = is_global;
    return value_;
}

const NJS::ValuePtr &NJS::Builder::GetVariable(const std::string &name) const
{
    auto only_is_global = false;
    for (auto &stack: std::ranges::reverse_view(m_Stack))
    {
        if (stack.Contains(name))
            if (const auto &[is_global_, value_] = stack[name];
                !only_is_global || is_global_)
                return value_;
        only_is_global |= stack.IsFunctionEntry;
    }
    Error("undefined symbol {}", name);
}

NJS::ValuePtr &NJS::Builder::GetOrDefineVariable(const std::string &name, const bool is_global)
{
    auto only_is_global = false;
    for (auto &stack: std::ranges::reverse_view(m_Stack))
    {
        if (stack.Contains(name))
            if (auto &[is_global_, value_] = stack[name];
                !only_is_global || is_global_)
                return value_;
        only_is_global |= stack.IsFunctionEntry;
    }

    auto &[is_global_, value_] = m_Stack.back()[name];
    is_global_ = is_global;
    return value_;
}

NJS::ReferenceInfo &NJS::Builder::CurrentFunctionResult()
{
    return m_Stack.back().Result;
}

llvm::BasicBlock *NJS::Builder::CurrentHeadBlock() const
{
    return m_Stack.back().HeadBlock;
}

llvm::BasicBlock *NJS::Builder::CurrentTailBlock() const
{
    return m_Stack.back().TailBlock;
}

llvm::Constant *NJS::Builder::GetString(const std::string &value)
{
    auto &pointer = m_StringTable[value];
    if (!pointer)
        pointer = GetBuilder().CreateGlobalStringPtr(value);
    return pointer;
}
