#include "lexer/lexer.hpp"

#include <assert.h>
#include <stdlib.h>

#include "lexFuncs.hpp"
#include "lexer/lexemes.hpp"

Lexer NewLexer(char* input)
{
    Token* toks = (Token*)calloc(64, sizeof(Token));
    assert(toks != NULL); // TODO: process error

    Lexer l = {
        .input      = input,
        .Tokens     = toks,
        .ToksCap    = 64,
        .currRow    = 1,
        .currColumn = 1,
    };

    return l;
}

void DestructLexer(Lexer* l)
{
    for (size_t i = 0; i < l->ToksLen; i++)
    {
        if (l->Tokens[i].n == TOK_ID)
            free((char*)l->Tokens[i].StrVal);
    }

    free(l->Tokens);
    return;
}

void Run(Lexer* l)
{
    lexProgramm(l);

    emit(l, TOK_EOF);
    return;
}
