#include "parser/parser.hpp"

#include <stdlib.h>

#include "nodes.hpp"
#include "parser.hpp"
#include "parser_expr.hpp"

Token* getCurrToken(Parser* p) { return &p->tokens[p->currTokenIdx]; }

bool currTokenHasName(Parser* p, TokenNamesSet n)
{
    if (p->HasErr)
        return false;

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

    if (p->tokens[p->currTokenIdx].n = TokEOF)
        return TokEOF;

    return p->tokens[p->currTokenIdx + 1].n;
}

static void funcParams(Parser* p, ListNode* ln)
{
    if (p->HasErr)
        return;

    if (!currTokenHasName(p, ID))
        return;

    size_t firstVarTokIdx = p->currTokenIdx;
    size_t firstVarIdx    = ln->chLen;

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

    Node ln = NewListNode(2, NODE_FUNCTION_PARAM);

    funcParams(p, ln.ln);

    while (currTokenHasName(p, COMMA))
    {
        eatToken(p, COMMA);
        funcParams(p, ln.ln);
    }

    return ln.ln;
}

static VarDeclNode* varDecl(Parser* p)
{
    if (p->HasErr)
        return NULL;

    eatToken(p, VAR);

    Token* varName = getCurrToken(p);
    eatToken(p, ID);

    Token* varType = getCurrToken(p);
    eatToken(p, FloatType | IntType | BoolType);

    Node init = {};

    if (currTokenHasName(p, ASSIGN))
    {
        eatToken(p, ASSIGN);
        init = expr(p);
    }

    return NewVarDeclNode(varName, varType, init).vdn;
}

static ListNode* compoundStmt(Parser* p) { eatToken(p, LBRACE); }

static FuncDeclNode* funcDecl(Parser* p)
{
    if (p->HasErr)
        return NULL;

    eatToken(p, FUNC);

    Token* fName = getCurrToken(p);
    eatToken(p, ID);

    eatToken(p, LPAREN);

    ListNode* params = funcParamsList(p);

    eatToken(p, RPAREN);

    Token* retType = NULL;
    if (!currTokenHasName(p, LBRACE))
    {
        retType = getCurrToken(p);
        eatToken(p, FloatType | IntType | BoolType);
    }

    ListNode* body = compoundStmt(p);

    Node fdn = NewFuncDeclNode(fName, params, retType, body);

    return fdn.fdn;
}

static Node programm(Parser* p)
{
    Node ln = NewListNode(2, NODE_PROGRAMM);

    // Node n =

    return ln;
}

Parser NewParser(Token* tokens)
{
    return Parser{
        .tokens = tokens,
    };
}
