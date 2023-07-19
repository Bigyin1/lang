/** @file */
#ifndef SEMANTIC_SYMTAB_PRIVATE_HPP
#define SEMANTIC_SYMTAB_PRIVATE_HPP

#include "parser/nodes.hpp"
#include "semantic/symtab.hpp"

const char* SymbolGetName(Symbol* sym);

Symbol* AddNewSymbol(SymTable* tab, Symbol* sym);

Symbol* GetSymbolByName(SymTable* tab, const char* name);

#endif
