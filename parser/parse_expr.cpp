#include "parse_expr.hpp"

#include <stdlib.h>

#include "nodes.hpp"
#include "parser.hpp"

static ListNode* funcArgs(Parser* p)
{
    if (p->HasErr)
        return NULL;

    Node ln = NewListNode(NODE_ARGS_LIST);

    Node arg = expr(p);
    ListNodeAddChild(ln.ln, arg);

    while (currTokenHasName(p, COMMA))
    {
        eatToken(p, COMMA);
        arg = expr(p);
        ListNodeAddChild(ln.ln, arg);
    }

    return ln.ln;
}

Node funccall(Parser* p)
{
    if (p->HasErr)
        return Node{};

    Token* fName = getCurrToken(p);
    eatToken(p, ID);

    eatToken(p, LPAREN);

    ListNode* args = NULL;
    if (!currTokenHasName(p, RPAREN))
        args = funcArgs(p);

    eatToken(p, RPAREN);

    return NewFuncCallNode(fName, args);
}

static Node factor(Parser* p)
{
    if (p->HasErr)
        return Node{};

    if (currTokenHasName(p, MINUS))
    {
        Token* op = getCurrToken(p);
        eatToken(p, MINUS);

        return NewUnOpNode(factor(p), op);
    }

    if (currTokenHasName(p, Integer | Float | TRUE | FALSE | ID))
    {
        if (currTokenHasName(p, ID) && peek(p) == LPAREN)
            return funccall(p);

        Token* val = getCurrToken(p);
        eatToken(p, val->n);

        return NewValNode(val);
    }

    if (!currTokenHasName(p, LPAREN))
    {
        emitError(p, LPAREN | Integer | Float | TRUE | FALSE | ID | MINUS);
        return Node{};
    }

    eatToken(p, LPAREN);

    Node exp = expr(p);

    eatToken(p, RPAREN);

    return exp;
}

static Node term(Parser* p)
{
    if (p->HasErr)
        return Node{};

    Node l = factor(p);

    while (currTokenHasName(p, MULT | DIV))
    {

        Token* op = getCurrToken(p);
        eatToken(p, op->n);

        Node r = factor(p);

        l = NewBinOpNode(l, r, op);
        if (p->HasErr)
            return l;
    }

    return l;
}

static Node arithm(Parser* p)
{
    if (p->HasErr)
        return Node{};

    Node l = term(p);

    while (currTokenHasName(p, PLUS | MINUS))
    {

        Token* op = getCurrToken(p);
        eatToken(p, op->n);

        Node r = term(p);

        l = NewBinOpNode(l, r, op);
        if (p->HasErr)
            return l;
    }

    return l;
}

Node expr(Parser* p)
{
    if (p->HasErr)
        return Node{};

    Node l = arithm(p);

    if (currTokenHasName(p, EQ | NE | GT | GE | LT | LE | NE))
    {
        Token* op = getCurrToken(p);
        eatToken(p, op->n);

        Node r = arithm(p);

        return NewBinOpNode(l, r, op);
    }

    return l;
}
