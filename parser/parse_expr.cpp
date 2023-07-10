#include <stdlib.h>

#include "nodes.hpp"
#include "parser.hpp"
#include "parser_expr.hpp"

static ListNode* funcArgs(Parser* p)
{

    Node ln = NewListNode(2, NODE_ARGS_LIST);

    Node arg = expr(p);
    ListNodeAddChild(ln.ln, arg);

    while (currTokenHasName(p, COMMA))
    {
        Node arg = expr(p);
        ListNodeAddChild(ln.ln, arg);
    }

    return ln.ln;
}

FuncCallNode* funccall(Parser* p)
{
    Token* fName = getCurrToken(p);
    eatToken(p, ID);

    eatToken(p, LPAREN);

    FuncCallNode* fcn = (FuncCallNode*)calloc(1, sizeof(FuncCallNode));
    fcn->fName        = fName;

    if (!currTokenHasName(p, RPAREN))
        fcn->args = funcArgs(p);

    eatToken(p, RPAREN);

    return fcn;
}

static Node factor(Parser* p)
{

    if (currTokenHasName(p, MINUS))
    {
        Token* op = getCurrToken(p);
        eatToken(p, MINUS);

        return NewUnOpNode(factor(p), op);
    }

    if (currTokenHasName(p, Integer | Float | TRUE | FALSE | ID))
    {
        if (currTokenHasName(p, ID) && peek(p) == LPAREN)
            return NewNodeWithType(funccall(p), NODE_FUNCTION_CALL);

        Token* val = getCurrToken(p);
        eatToken(p, val->n);

        return NewValNode(val);
    }

    eatToken(p, LPAREN);

    Node exp = expr(p);

    eatToken(p, RPAREN);

    return exp;
}

static Node term(Parser* p)
{

    Node l = factor(p);

    while (currTokenHasName(p, MULT | DIV))
    {

        Token* op = getCurrToken(p);
        eatToken(p, op->n);

        Node r = factor(p);

        l = NewBinOpNode(l, r, op);
    }

    return l;
}

static Node arithm(Parser* p)
{

    Node l = term(p);

    while (currTokenHasName(p, PLUS | MINUS))
    {

        Token* op = getCurrToken(p);
        eatToken(p, op->n);

        Node r = term(p);

        l = NewBinOpNode(l, r, op);
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
