/** @file */
#ifndef SCOPE_PRIVATE_HPP
#define SCOPE_PRIVATE_HPP

#include "semantic/scope.hpp"

ScopeNode* NewScopeNode();

void AddNewScope(ScopeNode* prev, ScopeNode* newScope);

Symbol* NewVarDef(ScopeNode* curr, Token* varTypeTok, Token* varNameTok);

Symbol* NewFuncDef(ScopeNode* curr, FuncDeclNode* fdn);

Type* DefineBaseType(ScopeNode* curr, BaseType bt, const char* name);

// Symbol* NewType(ScopeNode* curr, Node tn); TODO

#endif
