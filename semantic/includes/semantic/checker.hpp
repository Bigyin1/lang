/** @file */
#ifndef SEMANTIC_CHECKER_PUBLIC_HPP
#define SEMANTIC_CHECKER_PUBLIC_HPP

#include "semantic/errors.hpp"
#include "semantic/scope.hpp"

struct SemanticChecker
{
    ScopeNode* rootScope;

    FuncDeclNode* currFunc;
    ForStmtNode*  currCycle;

    SemanticError* errors;
    size_t         errsSz;
    size_t         errsCap;
};

void RunSemCheck(SemanticChecker* sch, ListNode* programm);

void FreeSemCheck(SemanticChecker* sch);

#endif
