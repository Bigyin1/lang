#include "lexer/error.hpp"

static const char* borrowErrStr(LexError* e)
{
    e->savedChar   = e->Str[e->Len];
    e->Str[e->Len] = '\0';

    return e->Str;
}

static void resetErrStr(LexError* e)
{
    e->Str[e->Len] = e->savedChar;
    return;
}

void Error(LexError* e, FILE* out)
{
    fprintf(out,
            "unknown lexeme at line"
            " %u column %u: %s\n",
            e->Row, e->Column, borrowErrStr(e));
    resetErrStr(e);
}
