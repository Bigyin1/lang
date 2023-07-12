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

Node NewNodeWithType(void* n, NodeType t)
{
    Node node     = {};
    node.hdr.type = t;

    node.gen = n;

    return node;
}
