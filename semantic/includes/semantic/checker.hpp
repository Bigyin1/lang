/** @file */
#ifndef SEMANTIC_CHECKER_PUBLIC_HPP
#define SEMANTIC_CHECKER_PUBLIC_HPP

#include "semantic/scope.hpp"

struct SemanticChecker
{
    ScopeNode* rootScope;

    FuncDeclNode* currFunc;
    ForStmtNode*  currCycle;
};

void RunSemCheck(SemanticChecker* sch, ListNode* programm);

#endif
