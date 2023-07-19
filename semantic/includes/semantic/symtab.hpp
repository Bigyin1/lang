/** @file */
#ifndef SEMANTIC_SYMTAB_PUBLIC_HPP
#define SEMANTIC_SYMTAB_PUBLIC_HPP

#include "parser/nodes.hpp"
#include "semantic/types.hpp"

enum SymbolClass
{
    SYM_VAR,
    SYM_FUNC,
    SYM_TYPE_DEF,
};

struct Symbol
{
    SymbolClass sclass;

    Type* type;

    Token*      tok;
    const char* name;
};

struct SymTable
{
    Symbol** symbols;
    size_t   symLen;
    size_t   symCap;
};

void FreeSymTable(SymTable* tab);

#endif
