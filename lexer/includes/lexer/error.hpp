/** @file */
#ifndef LEXER_ERROR_PUBLIC_HPP
#define LEXER_ERROR_PUBLIC_HPP

#include <stddef.h>
#include <stdio.h>

struct LexError
{
    unsigned Row;
    unsigned Column;
    char*    Str;
    size_t   Len;
    char     savedChar;
};

void Error(LexError* e, FILE* out);

#endif
