#include <llvm/IR/InlineAsm.h>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/value.hpp>

NJS::AsmExpression::AsmExpression(
    SourceLocation where,
    std::string asm_string,
    std::string constraints,
    const bool dialect_intel,
    const bool has_side_effects,
    const bool is_align_stack,
    const bool can_throw,
    std::vector<ExpressionPtr> arguments)
    : Expression(std::move(where)),
      AsmString(std::move(asm_string)),
      Constraints(std::move(constraints)),
      DialectIntel(dialect_intel),
      HasSideEffects(has_side_effects),
      IsAlignStack(is_align_stack),
      CanThrow(can_throw),
      Arguments(std::move(arguments))
{
}

std::ostream &NJS::AsmExpression::Print(std::ostream &stream) const
{
    stream << "asm<" << AsmString;
    if (!Constraints.empty())
        stream << " : " << Constraints;
    if (DialectIntel)
        stream << " : " << DialectIntel;
    if (HasSideEffects)
        stream << " : sideeffect";
    if (IsAlignStack)
        stream << " : alignstack";
    if (CanThrow)
        stream << " : throw";
    stream << ">";
    if (Arguments.empty())
        return stream;
    stream << "(";
    for (unsigned i = 0; i < Arguments.size(); ++i)
    {
        if (i > 0)
            stream << ", ";
        Arguments[i]->Print(stream);
    }
    return stream << ")";
}

NJS::ValuePtr NJS::AsmExpression::PGenLLVM(Builder &builder, const TypePtr &expected_type) const
{
    if (!expected_type)
        Error(
            Where,
            "inline assembler expression must be used by a statement or expression that expects or enforces a specified value type");

    const auto result_type = expected_type->GetLLVM(builder);

    std::vector<llvm::Type *> parameter_types;
    std::vector<llvm::Value *> argument_values;

    for (auto &argument: Arguments)
    {
        const auto value = argument->GenLLVM(builder, nullptr);
        parameter_types.emplace_back(value->GetType()->GetLLVM(builder));
        argument_values.emplace_back(value->Load());
    }

    const auto function_type = llvm::FunctionType::get(result_type, parameter_types, false);

    const auto asm_dialect = DialectIntel
                                 ? llvm::InlineAsm::AD_Intel
                                 : llvm::InlineAsm::AD_ATT;

    const auto inline_asm = llvm::InlineAsm::get(
        function_type,
        AsmString,
        Constraints,
        HasSideEffects,
        IsAlignStack,
        asm_dialect,
        CanThrow);

    const auto result = builder.GetBuilder().CreateCall(function_type, inline_asm, argument_values);
    return RValue::Create(builder, expected_type, result);
}
