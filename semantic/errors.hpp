/** @file */
#ifndef SEMANTIC_ERRORS_PRIVATE_HPP
#define SEMANTIC_ERRORS_PRIVATE_HPP

#include "semantic/checker.hpp"
#include "semantic/errors.hpp"

SemanticError NewUndefSymbErr(Token* symbol);

SemanticError NewSymbRedefErr(Token* symbol, Symbol* def);

SemanticError NewUndefOpErr(Token* op, const Type* t);

SemanticError NewUncallableSymbErr(Symbol* symbol);

SemanticError NewBadFuncCallNEArgs(Symbol* funcSymb, Token* fCall, size_t argsSz);

SemanticError NewBadRetType(const Type* ret, const Type* wanted, Token* retTok);

SemanticError NewFuncArgMismatch(Symbol* funcSymb, Token* fCall, const Type* argT, size_t argPos);

SemanticError NewJmpOutsideLoopErr(Token* jmp);

SemanticError NewTypesMismatchErr(const Type* t1, const Type* t2, Token* op);

SemanticError NewBadCondTypeErr(Token* cond, const Type* condType);

void AddNewErr(SemanticChecker* sch, SemanticError err);

#endif
