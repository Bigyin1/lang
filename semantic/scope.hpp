/** @file */
#ifndef SCOPE_PRIVATE_HPP
#define SCOPE_PRIVATE_HPP

#include "semantic/scope.hpp"

ScopeNode* NewScopeNode();

void AddNewScope(ScopeNode* prev, ScopeNode* newScope);

#endif
