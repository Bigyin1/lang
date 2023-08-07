#include "parser/nodes.hpp"

#include <stdlib.h>

#include "nodes.hpp"

Node NewListNode(NodeType nt)
{

    ListNode* ln = (ListNode*)calloc(1, sizeof(ListNode));
    ln->chCap    = 1;

    ln->children = (Node*)calloc(ln->chCap, sizeof(Node));

    return NewNodeWithType(ln, nt);
}

void ListNodeAddChild(ListNode* ln, Node child)
{

    if (ln->chLen == ln->chCap)
    {
        ln->chCap *= 2;
        ln->children = (Node*)realloc(ln->children, ln->chCap * sizeof(Node));
    }

    ln->children[ln->chLen] = child;

    ln->chLen++;
}

Node NewBinOpNode(Node l, Node r, Token* op)
{

    BinOpNode* bopn = (BinOpNode*)calloc(1, sizeof(BinOpNode));
    bopn->left      = l;
    bopn->right     = r;
    bopn->op        = op;

    return NewNodeWithType(bopn, NODE_BINOP);
}

Node NewUnOpNode(Node e, Token* op)
{

    UnOpNode* uopn = (UnOpNode*)calloc(1, sizeof(UnOpNode));
    uopn->child    = e;
    uopn->op       = op;

    return NewNodeWithType(uopn, NODE_UNOP);
}

Node NewValNode(Token* val)
{

    ValNode* vn = (ValNode*)calloc(1, sizeof(ValNode));
    vn->val     = val;

    return NewNodeWithType(vn, NODE_VAL);
}

Node NewVarDeclNode(Token* name, Token* type, Node init)
{

    VarDeclNode* vdn = (VarDeclNode*)calloc(1, sizeof(VarDeclNode));
    vdn->varName     = name;
    vdn->typeName    = type;
    vdn->initVal     = init;

    return NewNodeWithType(vdn, NODE_VAR_DECL);
}

Node NewIfStmtNode(Token* op, Node cond, ListNode* body, Node elseBody)
{

    IfStmtNode* ifstn = (IfStmtNode*)calloc(1, sizeof(IfStmtNode));

    ifstn->ifTok    = op;
    ifstn->cond     = cond;
    ifstn->body     = body;
    ifstn->elseBody = elseBody;

    return NewNodeWithType(ifstn, NODE_IF_STMT);
}

Node NewForStmtNode(Token* op, Node cond, ListNode* body)
{

    ForStmtNode* forstn = (ForStmtNode*)calloc(1, sizeof(ForStmtNode));

    forstn->forTok = op;
    forstn->cond   = cond;
    forstn->body   = body;

    return NewNodeWithType(forstn, NODE_FOR_STMT);
}

Node NewFuncDeclNode(Token* name, ListNode* params, Token* ret, ListNode* body)
{

    FuncDeclNode* fdn = (FuncDeclNode*)calloc(1, sizeof(FuncDeclNode));
    fdn->fName        = name;
    fdn->params       = params;
    fdn->retTypeName  = ret;
    fdn->body         = body;

    return NewNodeWithType(fdn, NODE_FUNCTION_DECL);
}

Node NewFuncParamNode(Token* paramName, Token* paramType)
{

    FuncParamNode* pn = (FuncParamNode*)calloc(1, sizeof(FuncParamNode));
    pn->paramName     = paramName;
    pn->typeName      = paramType;

    return NewNodeWithType(pn, NODE_FUNCTION_PARAM);
}

Node NewFuncCallNode(Token* name, ListNode* args)
{
    FuncCallNode* fcn = (FuncCallNode*)calloc(1, sizeof(FuncCallNode));
    fcn->fName        = name;

    fcn->args = args;

    return NewNodeWithType(fcn, NODE_FUNCTION_CALL);
}

Node NewNodeWithType(void* n, NodeType t)
{
    Node node     = {};
    node.hdr.type = t;

    node.rawPtr = n;

    return node;
}

static void freeListNode(ListNode* ln)
{
    if (ln == NULL)
        return;

    for (size_t i = 0; i < ln->chLen; i++)
        FreeNode(ln->children[i]);

    free(ln->children);

    free(ln);
}

static void freeBinOpNode(BinOpNode* bopn)
{
    if (bopn == NULL)
        return;

    FreeNode(bopn->left);
    FreeNode(bopn->right);

    free(bopn);
}

static void freeUnOpNode(UnOpNode* uopn)
{
    if (uopn == NULL)
        return;

    FreeNode(uopn->child);

    free(uopn);
}

static void freeValNode(ValNode* valn) { free(valn); }

static void freeVarDeclNode(VarDeclNode* vdn)
{
    if (vdn == NULL)
        return;

    FreeNode(vdn->initVal);

    free(vdn);
}

static void freeIfStmtNode(IfStmtNode* ifstn)
{
    if (ifstn == NULL)
        return;

    FreeNode(ifstn->cond);
    freeListNode(ifstn->body);
    FreeNode(ifstn->elseBody);

    free(ifstn);
}

static void freeForStmtNode(ForStmtNode* fstn)
{
    if (fstn == NULL)
        return;

    FreeNode(fstn->cond);
    freeListNode(fstn->body);

    free(fstn);
}

static void freeFuncDeclNode(FuncDeclNode* fdn)
{
    if (fdn == NULL)
        return;

    freeListNode(fdn->params);
    freeListNode(fdn->body);
    free(fdn);
}

static void freeFuncParamNode(FuncParamNode* fpn) { free(fpn); }

static void freeFuncCallNode(FuncCallNode* fcn)
{
    if (fcn == NULL)
        return;

    freeListNode(fcn->args);
    free(fcn);
}

void FreeNode(Node n)
{

    switch (n.hdr.type)
    {
        case NODE_EMPTY:
            return;

        case NODE_COMPOUND_STMT:
        case NODE_ARGS_LIST:
        case NODE_PARAMS_LIST:
        case NODE_PROGRAMM:
            freeListNode(n.ln);
            break;

        case NODE_BINOP:
            freeBinOpNode(n.bopn);
            break;
        case NODE_UNOP:
            freeUnOpNode(n.uopn);
            break;
        case NODE_VAL:
            freeValNode(n.vn);
            break;
        case NODE_VAR_DECL:
            freeVarDeclNode(n.vdn);
            break;
        case NODE_IF_STMT:
            freeIfStmtNode(n.ifstn);
            break;
        case NODE_FOR_STMT:
            freeForStmtNode(n.forstn);
            break;

        case NODE_FUNCTION_DECL:
            freeFuncDeclNode(n.fdn);
            break;

        case NODE_FUNCTION_PARAM:
            freeFuncParamNode(n.fpn);
            break;

        case NODE_FUNCTION_CALL:
            freeFuncCallNode(n.fcn);
            break;

        default:
            abort();
    }
}
