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
    const std::string &module_id,
    const bool is_main)
    : m_TypeContext(ctx),
      m_LLVMContext(context),
      m_ModuleID(module_id),
      m_IsMain(is_main)
{
    m_LLVMModule = std::make_unique<llvm::Module>(module_id, m_LLVMContext);
    m_LLVMBuilder = std::make_unique<llvm::IRBuilder<>>(m_LLVMContext);

    if (is_main)
        StackPush(m_ModuleID, m_TypeContext.GetIntegerType(32, true));
    else
        StackPush(m_ModuleID, m_TypeContext.GetVoidType());

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

    if (is_main)
    {
        const auto type = llvm::FunctionType::get(
            GetBuilder().getInt32Ty(),
            {GetBuilder().getInt32Ty(), GetBuilder().getPtrTy()},
            false);
        const auto function = llvm::Function::Create(
            type,
            llvm::GlobalValue::ExternalLinkage,
            "main",
            GetModule());
        function->getArg(0)->setName("argc");
        function->getArg(1)->setName("argv");
        GetBuilder().SetInsertPoint(llvm::BasicBlock::Create(GetContext(), "entry", function));
        CreateMember({}, process, "argc")->Store({}, function->getArg(0));
        CreateMember({}, process, "argv")->Store({}, function->getArg(1));
        return;
    }

    const auto type = llvm::FunctionType::get(GetBuilder().getVoidTy(), false);
    const auto function = llvm::Function::Create(
        type,
        llvm::GlobalValue::ExternalLinkage,
        GetName(true, "main"),
        GetModule());
    GetBuilder().SetInsertPoint(llvm::BasicBlock::Create(GetContext(), "entry", function));
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

void NJS::Builder::StackPush(const std::string &name, const TypePtr &result_type)
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

std::string NJS::Builder::GetName(const bool absolute, const std::string &name) const
{
    if (absolute)
        return m_ModuleID + '.' + std::string(name);
    return m_Stack.back().GetChildName(name);
}

void NJS::Builder::DefineFunction(const std::string &name, const FunctionTypePtr &type, llvm::Function *callee)
{
    m_FunctionMap[std::string(name)].emplace_back(type, callee);
}

NJS::FunctionInfo NJS::Builder::GetFunction(const std::string &name, const FunctionTypePtr &type) const
{
    if (!m_FunctionMap.contains(std::string(name)))
        return {};
    for (auto &info: m_FunctionMap.at(std::string(name)))
        if (info.Type == type)
            return info;
    return {};
}

NJS::FunctionInfo NJS::Builder::FindFunction(const std::string &name, const std::vector<ValuePtr> &arguments) const
{
    auto max_error = ~0u;
    FunctionInfo result_info;

    for (auto &info: m_FunctionMap.at(std::string(name)))
    {
        auto info_error = 0u;
        auto &[info_type_, info_callee_] = info;

        const auto parameter_count = info_type_->GetParameterCount();
        const auto count_difference = static_cast<int>(parameter_count) - static_cast<int>(arguments.size());
        if (count_difference > 0)
            continue;
        if (count_difference < 0 && !info_type_->IsVarArg())
            continue;

        info_error += abs(count_difference);

        unsigned i;
        for (i = 0; i < parameter_count; ++i)
        {
            const auto parameter_type = info_type_->GetParameterType(i);
            const auto argument_type = arguments[i]->GetType();
            if (parameter_type->IsReference() && !arguments[i]->IsLValue())
                break;
            const auto assignment_error = GetAssignmentError(parameter_type, argument_type);
            if (assignment_error == ~0u)
                break;
            info_error += assignment_error;
        }
        if (i < parameter_count)
            continue;

        if (info_error < max_error)
        {
            max_error = info_error;
            result_info = info;
        }
    }

    return result_info;
}

void NJS::Builder::DefineOperator(
    const std::string &name,
    const bool prefix,
    const TypePtr &value_type,
    const TypePtr &result_type,
    llvm::Value *callee)
{
    m_UnaryOperatorMap[std::string(name)][prefix][value_type] = {result_type, value_type, callee};
}

void NJS::Builder::DefineOperator(
    const std::string &name,
    const TypePtr &left_type,
    const TypePtr &right_type,
    const TypePtr &result_type,
    llvm::Value *callee)
{
    m_BinaryOperatorMap[std::string(name)][left_type][right_type] = {result_type, left_type, right_type, callee};
}

NJS::OperatorInfo<1> NJS::Builder::GetOperator(
    const std::string &name,
    const bool prefix,
    const TypePtr &value_type) const
{
    if (!m_UnaryOperatorMap.contains(std::string(name)))
        return {};
    auto &for_name = m_UnaryOperatorMap.at(std::string(name));
    if (!for_name.contains(prefix))
        return {};
    auto &for_prefix = for_name.at(prefix);
    if (!for_prefix.contains(value_type))
        return {};
    return for_prefix.at(value_type);
}

NJS::OperatorInfo<2> NJS::Builder::GetOperator(
    const std::string &name,
    const TypePtr &left_type,
    const TypePtr &right_type) const
{
    if (!m_BinaryOperatorMap.contains(std::string(name)))
        return {};
    auto &for_name = m_BinaryOperatorMap.at(std::string(name));
    if (!for_name.contains(left_type))
        return {};
    auto &for_left = for_name.at(left_type);
    if (!for_left.contains(right_type))
        return {};
    return for_left.at(right_type);
}

NJS::OperatorInfo<1> NJS::Builder::FindOperator(
    const std::string &name,
    const bool prefix,
    const ValuePtr &value) const
{
    const auto value_type = value->GetType();
    const auto value_type_ref = value->IsLValue()
                                    ? GetTypeContext().GetReferenceType(value_type)
                                    : value_type;
    if (auto o = GetOperator(name, prefix, value_type_ref); o.Callee)
        return o;
    if (auto o = GetOperator(name, prefix, value_type); o.Callee)
        return o;
    return {};
}

NJS::OperatorInfo<2> NJS::Builder::FindOperator(
    const std::string &name,
    const ValuePtr &left_operand,
    const ValuePtr &right_operand) const
{
    const auto left_type = left_operand->GetType();
    const auto right_type = right_operand->GetType();
    const auto left_type_ref = left_operand->IsLValue()
                                   ? GetTypeContext().GetReferenceType(left_type)
                                   : left_type;
    const auto right_type_ref = right_operand->IsLValue()
                                    ? GetTypeContext().GetReferenceType(right_type)
                                    : right_type;
    if (auto o = GetOperator(name, left_type_ref, right_type_ref); o.Callee)
        return o;
    if (auto o = GetOperator(name, left_type, right_type_ref); o.Callee)
        return o;
    if (auto o = GetOperator(name, left_type_ref, right_type); o.Callee)
        return o;
    if (auto o = GetOperator(name, left_type, right_type); o.Callee)
        return o;
    return {};
}

NJS::ValuePtr &NJS::Builder::DefineVariable(const SourceLocation &where, const std::string &name)
{
    auto &stack = m_Stack.back();
    if (stack.Contains(name))
        Error(where, "cannot redefine symbol '{}'", name);
    return stack[name];
}

NJS::ValuePtr &NJS::Builder::GetVariable(const SourceLocation &where, const std::string &name)
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
