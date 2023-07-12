#include "parse_programm.hpp"

#include <stdlib.h>

#include "nodes.hpp"
#include "parse_expr.hpp"
#include "parse_stmt.hpp"
#include "parser.hpp"
#include "parser/parser.hpp"

static void funcParams(Parser* p, ListNode* ln)
{

    if (!currTokenHasName(p, ID))
        return;

    size_t firstVarIdx = ln->chLen;

    while (currTokenHasName(p, ID))
    {
        FuncParamNode* pn = (FuncParamNode*)calloc(1, sizeof(FuncParamNode));
        pn->paramName     = getCurrToken(p);

        ListNodeAddChild(ln, NewNodeWithType(pn, NODE_FUNCTION_PARAM));

        eatToken(p, ID);

        if (!currTokenHasName(p, COMMA))
            break;

        eatToken(p, COMMA);
    }

    Token* type = getCurrToken(p);

    eatToken(p, FloatType | IntType | BoolType);

    for (size_t i = firstVarIdx; i < ln->chLen; i++)
    {
        ln->children[i].fpn->typeName = type;
    }
}

static ListNode* funcParamsList(Parser* p)
{
    if (p->HasErr)
        return NULL;

    Node ln = NewListNode(NODE_PARAMS_LIST);

    funcParams(p, ln.ln);

    while (currTokenHasName(p, COMMA))
    {
        eatToken(p, COMMA);
        funcParams(p, ln.ln);
    }

    return ln.ln;
}

static Node funcDecl(Parser* p)
{

    eatToken(p, FUNC);

    Token* fName = getCurrToken(p);
    eatToken(p, ID);
    if (p->HasErr)
        return Node{};

    eatToken(p, LPAREN);

    ListNode* params = funcParamsList(p);

    eatToken(p, RPAREN);

    Token* retType = NULL;
    if (!currTokenHasName(p, LBRACE))
    {
        retType = getCurrToken(p);
        eatToken(p, FloatType | IntType | BoolType);
    }

    ListNode* body = compoundStmt(p).ln;

    return NewFuncDeclNode(fName, params, retType, body);
}

Node programm(Parser* p)
{
    Node ln = NewListNode(NODE_PROGRAMM);

    while (currTokenHasName(p, FUNC))
    {
        ListNodeAddChild(ln.ln, funcDecl(p));
    }

    return ln;
}
