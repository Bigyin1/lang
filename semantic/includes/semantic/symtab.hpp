/** @file */
#ifndef SEMANTIC_SYMTAB_PUBLIC_HPP
#define SEMANTIC_SYMTAB_PUBLIC_HPP

#include "parser/nodes.hpp"

struct Symbol
{
    union
    {
        TokenName     Type;
        FuncDeclNode* FuncType;
    };

    Token*      Tok;
    const char* Name;
};

struct SymTable
{
    Symbol* symbols;
    size_t  symLen;
    size_t  symCap;
};

Symbol* GetSymbolByName(SymTable* tab, const char* name);

Symbol* AddNewSymbol(SymTable* tab, Symbol sym);

#endif
