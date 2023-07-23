#include "parse_programm.hpp"

#include <stdlib.h>

#include "nodes.hpp"
#include "parse_expr.hpp"
#include "parse_stmt.hpp"
#include "parser.hpp"
#include "parser/parser.hpp"

static void funcParams(Parser* p, ListNode* ln)
{
    if (!currTokenHasName(p, TOK_ID))
        return;

    size_t firstVarIdx = ln->chLen;

    while (currTokenHasName(p, TOK_ID))
    {
        Token* paramName = getCurrToken(p);
        ListNodeAddChild(ln, NewFuncParamNode(paramName, NULL));

        eatToken(p, TOK_ID);

        if (!currTokenHasName(p, TOK_COMMA))
            break;

        eatToken(p, TOK_COMMA);
    }

    Token* type = getCurrToken(p);

    eatToken(p, TOK_FloatType | TOK_IntType | TOK_BoolType);

    for (size_t i = firstVarIdx; i < ln->chLen; i++)
        ln->children[i].fpn->typeName = type;
}

static ListNode* funcParamsList(Parser* p)
{

    Node ln = NewListNode(NODE_PARAMS_LIST);

    funcParams(p, ln.ln);

    while (currTokenHasName(p, TOK_COMMA))
    {
        eatToken(p, TOK_COMMA);
        funcParams(p, ln.ln);
        if (p->HasErr)
            return ln.ln;
    }

    return ln.ln;
}

static Node funcDecl(Parser* p)
{
    eatToken(p, TOK_FUNC);

    Token* fName = getCurrToken(p);
    eatToken(p, TOK_ID);

    eatToken(p, TOK_LPAREN);

    ListNode* params = funcParamsList(p);

    eatToken(p, TOK_RPAREN);

    Token* retType = NULL;
    if (currTokenHasName(p, TOK_FloatType | TOK_IntType | TOK_BoolType))
    {
        retType = getCurrToken(p);
        eatToken(p, TOK_FloatType | TOK_IntType | TOK_BoolType);
    }

    ListNode* body = compoundStmt(p).ln;

    return NewFuncDeclNode(fName, params, retType, body);
}

Node programm(Parser* p)
{
    Node ln = NewListNode(NODE_PROGRAMM);

    while (currTokenHasName(p, TOK_FUNC))
    {
        ListNodeAddChild(ln.ln, funcDecl(p));
    }

    return ln;
}
