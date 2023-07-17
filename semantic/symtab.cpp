#include "semantic/symtab.hpp"

#include <stdlib.h>
#include <string.h>

#include "scope.hpp"

Symbol* GetSymbolByName(SymTable* tab, const char* name)
{

    for (size_t i = 0; i < tab->symLen; i++)
    {
        if (strcmp(name, tab->symbols[i].Name) == 0)
            return &tab->symbols[i];
    }

    return NULL;
}

Symbol* AddNewSymbol(SymTable* tab, Symbol sym)
{
    Symbol* dup = GetSymbolByName(tab, sym.Name);
    if (dup != NULL)
        return dup;

    if (tab->symCap == 0)
    {
        tab->symCap  = 1;
        tab->symbols = (Symbol*)calloc(tab->symCap, sizeof(Symbol));
    }

    if (tab->symCap == tab->symLen)
    {
        tab->symCap *= 2;
        tab->symbols = (Symbol*)realloc(tab->symbols, sizeof(Symbol) * tab->symCap);
    }

    tab->symbols[tab->symLen] = sym;

    tab->symLen++;

    return NULL;
}
