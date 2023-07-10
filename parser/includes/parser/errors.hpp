/** @file */
#ifndef PARSER_ERROR_PUBLIC_HPP
#define PARSER_ERROR_PUBLIC_HPP

#include <stdio.h>

#include "parser/parser.hpp"

typedef uint64_t TokenNamesSet;

struct UnexpTokenError
{
    Token*        tok;
    TokenNamesSet exp;
};

void Error(UnexpTokenError* e, FILE* out);

#endif
