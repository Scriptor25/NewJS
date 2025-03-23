#include <llvm/IR/InlineAsm.h>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

NJS::AsmExpression::AsmExpression(
    SourceLocation where,
    std::string source,
    std::string constraints,
    const bool is_intel,
    const bool is_volatile,
    const bool is_align,
    const bool can_throw,
    std::vector<ExpressionPtr> operands,
    TypePtr output_type)
    : Expression(std::move(where)),
      Source(std::move(source)),
      Constraints(std::move(constraints)),
      IsIntel(is_intel),
      IsVolatile(is_volatile),
      IsAlign(is_align),
      IsThrow(can_throw),
      Operands(std::move(operands)),
      OutputType(std::move(output_type))
{
}

std::ostream &NJS::AsmExpression::Print(std::ostream &stream) const
{
    stream << "asm";
    if (IsVolatile)
        stream << " volatile";
    if (IsAlign)
        stream << " align";
    if (IsThrow)
        stream << " throw";
    if (IsIntel)
        stream << " intel";
    stream << '(' << Source;
    auto constraints = Constraints;
    if (OutputType)
    {
        const auto pos = constraints.find(',');
        const auto constraint = constraints.substr(0, pos);
        constraints = constraints.substr(pos + 1);

        stream << " : ";
        stream << '"' << constraint << '"';
        stream << '<';
        OutputType->Print(stream);
        stream << '>';
    }
    if (!Operands.empty())
    {
        stream << " : ";
        for (unsigned i = 0; i < Operands.size(); ++i)
        {
            const auto pos = constraints.find(',');
            const auto constraint = constraints.substr(0, pos);
            constraints = constraints.substr(pos + 1);

            if (i > 0)
                stream << ", ";

            stream << '"' << constraint << '"';
            stream << '(';
            Operands[i]->Print(stream);
            stream << ')';
        }
    }
    return stream << ')';
}

NJS::ValuePtr NJS::AsmExpression::PGenLLVM(Builder &builder, const TypePtr &)
{
    std::vector<llvm::Type *> operand_types;
    std::vector<llvm::Value *> operands;

    for (const auto &argument: Operands)
    {
        const auto value = argument->GenLLVM(builder, nullptr);
        operand_types.emplace_back(value->GetType()->GetLLVM(builder));
        operands.emplace_back(value->Load());
    }

    const auto output_type = OutputType
                                 ? OutputType
                                 : builder.GetTypeContext().GetVoidType();

    const auto result_type = output_type->GetLLVM(builder);
    const auto function_type = llvm::FunctionType::get(result_type, operand_types, false);

    const auto asm_dialect = IsIntel
                                 ? llvm::InlineAsm::AD_Intel
                                 : llvm::InlineAsm::AD_ATT;

    const auto inline_asm = llvm::InlineAsm::get(
        function_type,
        Source,
        Constraints,
        IsVolatile,
        IsAlign,
        asm_dialect,
        IsThrow);

    const auto result = builder.GetBuilder().CreateCall(function_type, inline_asm, operands);
    return RValue::Create(builder, output_type, result);
}
