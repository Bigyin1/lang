/** @file */
#ifndef SEMANTIC_TYPES_PUBLIC_HPP
#define SEMANTIC_TYPES_PUBLIC_HPP

#include "stddef.h"

enum TypeClass
{
    TypeClassBase = 1,
    TypeClassFunc,
    // TypeClassStruct,  TODO
};

enum BaseType
{
    BaseTypeInteger,
    BaseTypeFloat64,
    BaseTypeBoolean,
};

struct Type;

struct FuncType
{
    const Type* paramTypes[10];
    size_t      paramsSz;

    const Type* retType;
};

struct Type
{
    TypeClass tclass;

    union
    {
        BaseType bt;
        FuncType ft;
        // StructType st;  TODO
    };

    const char* name;
};

#endif
