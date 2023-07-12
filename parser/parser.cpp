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

void eatToken(Parser* p, TokenNamesSet exp)
{
    if (p->HasErr)
        return;

    if (currTokenHasName(p, exp))
    {
        p->currTokenIdx++;
        return;
    }
    p->Err = UnexpTokenError{
        .tok = getCurrToken(p),
        .exp = exp,
    };

    p->HasErr = true;
}

TokenName peek(Parser* p)
{
    if (p->HasErr)
        return TokEOF;

    if (p->tokens[p->currTokenIdx].n == TokEOF)
        return TokEOF;

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
