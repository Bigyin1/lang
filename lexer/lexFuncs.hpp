/** @file */
#ifndef LEX_FUNCS_HPP
#define LEX_FUNCS_HPP

#include <stddef.h>
#include <stdio.h>

#include "lexer/lexer.hpp"

char next(Lexer* l);

const char* getCurrLexeme(Lexer* l);

void resetCurrLexeme(Lexer* l);

void ignore(Lexer* l);

typedef int (*acceptFn)(int);

void backup(Lexer* l);

void acceptRun(Lexer* l, acceptFn fn);

bool acceptSet(Lexer* l, const char* valid);

void emit(Lexer* l, TokenName tName);

void emitErr(Lexer* l);

void lexNumber(Lexer* l);

void lexKeywordOrId(Lexer* l);

void lexNonAlphaReservedWords(Lexer* l);

void lexProgramm(Lexer* l);

#endif
