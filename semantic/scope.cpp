#include "scope.hpp"

#include <stdlib.h>

ScopeNode* NewScopeNode()
{
    ScopeNode* n = (ScopeNode*)calloc(1, sizeof(ScopeNode));

    return n;
}

void AddNewScope(ScopeNode* prev, ScopeNode* newScope)
{

    if (newScope == NULL)
        return;

    if (prev->chCap == 0)
    {
        prev->chCap    = 1;
        prev->children = (ScopeNode**)calloc(prev->chCap, sizeof(ScopeNode*));
    }

    if (prev->chCap == prev->chLen)
    {
        prev->chCap *= 2;
        prev->children = (ScopeNode**)realloc(prev->children, sizeof(ScopeNode*) * prev->chCap);
    }

    newScope->prev              = prev;
    prev->children[prev->chLen] = newScope;

    prev->chLen++;
}

Symbol* GetSymbolByNameFromScope(ScopeNode* sc, const char* name)
{
    Symbol* res = NULL;

    ScopeNode* currScope = sc;
    while (currScope != NULL)
    {
        res = GetSymbolByName(&currScope->map, name);
        if (res != NULL)
            return res;

        currScope = currScope->prev;
    }

    return NULL;
}
