#include "parser/errors.hpp"

void PrintParserError(UnexpTokenError* e, FILE* out)
{

    fprintf(out, "unexpected token ");

    switch (e->tok->n)
    {
        case ID:
            fprintf(out, "%s", e->tok->StrVal);
            break;

        case Integer:
            fprintf(out, "%lu", e->tok->IntVal);
            break;

        case Float:
            fprintf(out, "%lf", e->tok->FloatVal);
            break;

        default:
            fprintf(out, "%s", TokenNameToString(e->tok->n));
            break;
    }

    fprintf(out, " at line: %u; column: %u\n", e->tok->Row, e->tok->Col);
    fprintf(out, "expected one of tokens: ");

    for (size_t i = 0; i < sizeof(TokenNames) / sizeof(TokenName); i++)
    {
        if (TokenNames[i] & e->exp)
            fprintf(out, " %s", TokenNameToString(TokenNames[i]));
    }

    fprintf(out, "\n");
}
