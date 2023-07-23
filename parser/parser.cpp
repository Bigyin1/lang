#include "parser/parser.hpp"

#include <stdlib.h>

#include "parse_programm.hpp"

Token* getCurrToken(Parser* p) { return &p->tokens[p->currTokenIdx]; }

bool currTokenHasName(Parser* p, TokenNamesSet n)
{

    TokenName currName = getCurrToken(p)->n;
    if (n & currName)
        return true;

    return false;
}

void emitError(Parser* p, TokenNamesSet exp)
{
    p->Err = UnexpTokenError{
        .tok = getCurrToken(p),
        .exp = exp,
    };

    p->HasErr = true;
}

void eatToken(Parser* p, TokenNamesSet exp)
{
    if (p->HasErr)
        return;

    if (currTokenHasName(p, exp))
    {
        p->currTokenIdx++;
        return;
    }
    emitError(p, exp);
}

TokenName peek(Parser* p)
{
    if (p->HasErr)
        return TOK_EOF;

    if (p->tokens[p->currTokenIdx].n == TOK_EOF)
        return TOK_EOF;

    return p->tokens[p->currTokenIdx + 1].n;
}

Parser NewParser(Token* tokens)
{
    return Parser{
        .tokens = tokens,
    };
}

void Run(Parser* p)
{
    p->prog = programm(p);
    return;
}
