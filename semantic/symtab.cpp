#include "semantic/symtab.hpp"

#include <stdlib.h>
#include <string.h>

#include "scope.hpp"
#include "symtab.hpp"

const char* SymbolGetName(Symbol* sym) { return sym->name; }

Symbol* AddNewSymbol(SymTable* tab, Symbol* sym)
{
    Symbol* dup = GetSymbolByName(tab, SymbolGetName(sym));
    if (dup != NULL)
        return dup;

    if (tab->symCap == 0)
    {
        tab->symCap  = 1;
        tab->symbols = (Symbol**)calloc(tab->symCap, sizeof(Symbol*));
    }

    if (tab->symCap == tab->symLen)
    {
        tab->symCap *= 2;
        tab->symbols = (Symbol**)realloc(tab->symbols, sizeof(Symbol*) * tab->symCap);
    }

    tab->symbols[tab->symLen] = sym;

    tab->symLen++;

    return NULL;
}

Symbol* GetSymbolByName(SymTable* tab, const char* name)
{

    for (size_t i = 0; i < tab->symLen; i++)
    {
        if (strcmp(name, SymbolGetName(tab->symbols[i])) == 0)
            return tab->symbols[i];
    }

    return NULL;
}

void FreeSymTable(SymTable* tab)
{

    for (size_t i = 0; i < tab->symLen; i++)
    {
        Symbol* curr = tab->symbols[i];
        if (curr->sclass == SYM_FUNC || curr->sclass == SYM_TYPE_DEF)
            free(curr->type);

        free(curr);
    }

    free(tab->symbols);
}
