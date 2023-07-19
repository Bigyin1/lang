/** @file */
#ifndef SEMANTIC_SCOPE_PUBLIC_HPP
#define SEMANTIC_SCOPE_PUBLIC_HPP

#include "semantic/symtab.hpp"

struct ScopeNode
{
    ScopeNode* prev;

    ScopeNode** children;
    size_t      chLen;
    size_t      chCap;

    SymTable map;

    size_t currChild;
};

Symbol* GetSymbolByNameFromScope(ScopeNode* sc, const char* name);

Type* GetTypeByNameFromScope(ScopeNode* sc, const char* tname);

void FreeScope(ScopeNode* sc);

#endif
