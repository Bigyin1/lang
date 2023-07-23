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

    while (currTokenHasName(p, TOK_COMMA))
    {
        eatToken(p, TOK_COMMA);
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
    eatToken(p, TOK_ID);

    eatToken(p, TOK_LPAREN);

    ListNode* args = NULL;
    if (!currTokenHasName(p, TOK_RPAREN))
        args = funcArgs(p);

    eatToken(p, TOK_RPAREN);

    return NewFuncCallNode(fName, args);
}

static Node factor(Parser* p)
{
    if (p->HasErr)
        return Node{};

    if (currTokenHasName(p, TOK_MINUS | TOK_NOT))
    {
        Token* op = getCurrToken(p);
        eatToken(p, TOK_MINUS | TOK_NOT);

        return NewUnOpNode(factor(p), op);
    }

    if (currTokenHasName(p, TOK_Integer | TOK_Float | TOK_TRUE | TOK_FALSE | TOK_ID))
    {
        if (currTokenHasName(p, TOK_ID) && peek(p) == TOK_LPAREN)
            return funccall(p);

        Token* val = getCurrToken(p);
        eatToken(p, val->n);

        return NewValNode(val);
    }

    if (!currTokenHasName(p, TOK_LPAREN))
    {
        emitError(p, TOK_LPAREN | TOK_Integer | TOK_Float | TOK_TRUE | TOK_FALSE | TOK_ID |
                         TOK_MINUS | TOK_NOT);
        return Node{};
    }

    eatToken(p, TOK_LPAREN);

    Node exp = expr(p);

    eatToken(p, TOK_RPAREN);

    return exp;
}

static Node term(Parser* p)
{
    if (p->HasErr)
        return Node{};

    Node l = factor(p);

    while (currTokenHasName(p, TOK_MULT | TOK_DIV | TOK_LAND))
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

    while (currTokenHasName(p, TOK_PLUS | TOK_MINUS | TOK_LOR))
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

    if (currTokenHasName(p, TOK_EQ | TOK_NE | TOK_GT | TOK_GE | TOK_LT | TOK_LE | TOK_NE))
    {
        Token* op = getCurrToken(p);
        eatToken(p, op->n);

        Node r = arithm(p);

        return NewBinOpNode(l, r, op);
    }

    return l;
}
