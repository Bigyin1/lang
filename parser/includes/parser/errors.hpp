/** @file */
#ifndef PARSER_ERROR_PUBLIC_HPP
#define PARSER_ERROR_PUBLIC_HPP

#include <stdio.h>

#include "lexer/lexer.hpp"

typedef uint64_t TokenNamesSet;

struct UnexpTokenError
{
    Token*        tok;
    TokenNamesSet exp;
};

void PrintParserError(UnexpTokenError* e, FILE* out);

#endif
