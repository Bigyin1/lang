#include "lexer/lexer.hpp"

#include <stdlib.h>
#include <string.h>

#include "lexer/lexemes.hpp"

const char* testData = "int a\n"
                       "float b = 3.14\n"
                       "while a>= b";

Token expToks[] = {

    {.n = IntType, .Row = 1, .Col = 1},   {.n = ID, .StrVal = "a", .Row = 1, .Col = 5},
    {.n = FloatType, .Row = 2, .Col = 1}, {.n = ID, .StrVal = "b", .Row = 2, .Col = 7},
    {.n = Assign, .Row = 2, .Col = 9},    {.n = Float, .FloatVal = 3.14, .Row = 2, .Col = 11},
    {.n = While, .Row = 3, .Col = 1},     {.n = ID, .StrVal = "a", .Row = 3, .Col = 7},
    {.n = GE, .Row = 3, .Col = 8},        {.n = ID, .StrVal = "b", .Row = 3, .Col = 11},

};

int main()
{
    char* d = strdup(testData);

    Lexer l = NewLexer(d);

    Run(&l);

    if (l.HasErrs)
    {
        printf("failed: ");
        Error(&l.Err, stdout);
        return 1;
    }

    free(d);
    if (sizeof(expToks) / sizeof(Token) != l.ToksLen)
    {
        printf("unexpected tokens count: %zu, exp: %zu\n", l.ToksLen,
               sizeof(expToks) / sizeof(Token));
        return 1;
    }

    for (size_t i = 0; i < l.ToksLen; i++)
    {
        if (l.Tokens[i].n != expToks[i].n)
        {
            printf("unexpected token type(idx: %zu): %s, exp: %s\n", i,
                   TokenNameToString(l.Tokens[i].n), TokenNameToString(expToks[i].n));
            return 1;
        }

        if (l.Tokens[i].n == ID)
        {
            if (strcmp(l.Tokens[i].StrVal, expToks[i].StrVal) != 0)
            {
                printf("unexpected ID token value(idx: %zu): %s, exp: %s\n", i, l.Tokens[i].StrVal,
                       expToks[i].StrVal);
                return 1;
            }
        }

        if (l.Tokens[i].n == Float)
        {
            if (l.Tokens[i].IntVal != expToks[i].IntVal)
            {
                printf("unexpected float token value(idx: %zu): %lf, exp: %lf\n", i,
                       l.Tokens[i].FloatVal, expToks[i].FloatVal);
                return 1;
            }
        }
        if (l.Tokens[i].n == Integer)
        {
            if (l.Tokens[i].IntVal != expToks[i].IntVal)
            {
                printf("unexpected integer token value(idx: %zu): %lu, exp: %lu\n", i,
                       l.Tokens[i].IntVal, expToks[i].IntVal);
                return 1;
            }
        }
    }

    DestructLexer(&l);
}
