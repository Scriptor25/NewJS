#pragma once

#include <NJS/NJS.hpp>

namespace NJS
{
    ValuePtr OperatorEQ(Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr OperatorNE(Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr OperatorLT(Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr OperatorLE(Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr OperatorGT(Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr OperatorGE(Builder&, const ValuePtr&, const ValuePtr&);

    ValuePtr OperatorLOr(Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr OperatorLXor(Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr OperatorLAnd(Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr OperatorOr(Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr OperatorXor(Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr OperatorAnd(Builder&, const ValuePtr&, const ValuePtr&);

    ValuePtr OperatorAdd(Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr OperatorSub(Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr OperatorMul(Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr OperatorDiv(Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr OperatorRem(Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr OperatorPow(Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr OperatorRoot(Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr OperatorShL(Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr OperatorShR(Builder&, const ValuePtr&, const ValuePtr&);
}
