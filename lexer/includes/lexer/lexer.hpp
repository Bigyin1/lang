/** @file */
#ifndef LEXER_PUBLIC_HPP
#define LEXER_PUBLIC_HPP

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "lexer/error.hpp"
#include "lexer/lexemes.hpp"

typedef uint16_t TokenClass;

struct Token
{
    TokenName n;

    union
    {
        const char* StrVal;
        double      FloatVal;
        int64_t     IntVal;
    };

    unsigned Row;
    unsigned Col;
};

// void PrintToken(Token* tok, FILE* out) {

// }

struct Lexer
{
    char* input;

    unsigned start; // start position for the current lexeme
    unsigned pos;   // current position (>= start)

    char savedChar;

    Token* Tokens;
    size_t ToksLen;
    size_t ToksCap;

    LexError Err;
    bool     HasErrs;

    unsigned currRow;
    unsigned currColumn;
};

Lexer NewLexer(char* input);

void DestructLexer(Lexer* l);

void Run(Lexer* l);

#endif
