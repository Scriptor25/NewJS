#pragma once

#include <NJS/NJS.hpp>

namespace NJS
{
    ValuePtr OperatorLE(Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr OperatorAdd(Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr OperatorSub(Builder&, const ValuePtr&, const ValuePtr&);
}
