#include "lexFuncs.hpp"

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/lexemes.hpp"

char next(Lexer* l)
{
    if (l->input[l->pos] == '\0')
    {
        return 0;
    }

    char r = l->input[l->pos];
    l->pos++;

    return r;
}

const char* getCurrLexeme(Lexer* l)
{
    l->savedChar     = l->input[l->pos];
    l->input[l->pos] = '\0';

    return l->input + l->start;
}

void resetCurrLexeme(Lexer* l) { l->input[l->pos] = l->savedChar; }

void ignore(Lexer* l)
{
    l->currColumn += (l->pos - l->start);
    l->start = l->pos;
}

void backup(Lexer* l) { l->pos -= 1; }

typedef int (*acceptFn)(int);

void acceptRun(Lexer* l, acceptFn fn)
{
    char c = 0;

    for (;;)
    {
        c = next(l);
        if (!fn(c))
            break;
    }

    if (c != 0)
        backup(l);
}

bool acceptSet(Lexer* l, const char* valid)
{
    char c = next(l);

    if (strchr(valid, c))
        return true;

    if (c != 0)
        backup(l);

    return false;
}

void emit(Lexer* l, TokenName tName)
{
    size_t newTokIdx = l->ToksLen;
    l->ToksLen++;

    if (l->ToksLen > l->ToksCap)
    {
        l->ToksCap *= 2;
        l->Tokens = (Token*)realloc(l->Tokens, sizeof(Token) * l->ToksCap);
    }

    l->Tokens[newTokIdx] = Token{
        .n   = tName,
        .Row = l->currRow,
        .Col = l->currColumn,
    };

    const char* currLex = getCurrLexeme(l);
    switch (tName)
    {
        case TOK_ID:
            l->Tokens[newTokIdx].StrVal = strdup(currLex);
            break;

        case TOK_Integer:
        {
            uint64_t val = 0;
            sscanf(currLex, "%lu", &val);
            l->Tokens[newTokIdx].IntVal = val;
            break;
        }
        case TOK_Float:
        {
            double val = 0;
            sscanf(currLex, "%lf", &val);
            l->Tokens[newTokIdx].FloatVal = val;
            break;
        }
    }

    resetCurrLexeme(l);

    ignore(l);
}

void emitErr(Lexer* l)
{
    l->Err = {
        // TODO: add multiple errors
        .Row    = l->currRow,
        .Column = l->currColumn,
        .Str    = l->input + l->start,
        .Len    = l->pos - l->start,
    };

    l->HasErrs = true;

    ignore(l);
}

void lexNumber(Lexer* l)
{

    acceptRun(l, isdigit);

    if (acceptSet(l, "."))
    {
        acceptRun(l, isdigit);
        emit(l, TOK_Float);
    }
    else
    {
        emit(l, TOK_Integer);
    }
}

void lexKeywordOrId(Lexer* l)
{
    acceptRun(l, isalnum);

    const char* lexeme = getCurrLexeme(l);
    TokenName   tn     = ReservedWordToToken(lexeme);
    resetCurrLexeme(l);

    if (tn == TOK_EOF) // got ID
    {
        emit(l, TOK_ID);
        return;
    }

    emit(l, tn);
}

void lexNonAlphaReservedWords(Lexer* l)
{
    acceptRun(l, ispunct);

    while (l->start != l->pos)
    {
        const char* lexeme = getCurrLexeme(l);
        TokenName   tn     = ReservedWordToToken(lexeme);
        resetCurrLexeme(l);

        if (tn == TOK_EOF)
        {
            backup(l);
            continue;
        }
        emit(l, tn);
        return;
    }

    acceptRun(l, ispunct);
    emitErr(l);
}

void lexProgramm(Lexer* l)
{
    for (;;)
    {
        if (l->HasErrs) // TODO: collect all errors
            return;

        char c = next(l);
        if (c == '\0')
            return;

        if (isdigit(c))
        {
            backup(l);
            lexNumber(l);
        }
        else if (isspace(c))
        {
            ignore(l);
            if (c == '\n')
            {
                l->currRow += 1;
                l->currColumn = 1;
            }
        }
        else if (isalpha(c))
        {
            backup(l);
            lexKeywordOrId(l);
        }
        else
        {
            backup(l);
            lexNonAlphaReservedWords(l);
        }
    }
}
