#pragma once

namespace NJS
{
    enum TypeId
    {
        /** primitive void type */
        TypeId_Void,
        /** primitive boolean type */
        TypeId_Boolean,
        /** primitive number type */
        TypeId_Number,
        /** primitive string type */
        TypeId_String,
        /** complex type, consisting of a pointer to an info structure and some data */
        TypeId_Complex,
    };
}
