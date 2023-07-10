#include "parser/nodes.hpp"

#include <stdlib.h>

#include "nodes.hpp"

Node NewListNode(size_t initCap, NodeType nt)
{

    ListNode* ln = (ListNode*)calloc(1, sizeof(ListNode));
    ln->chCap    = initCap;

    ln->children = (Node*)calloc(ln->chCap, sizeof(Node*));

    return NewNodeWithType(ln, nt);
}

void ListNodeAddChild(ListNode* ln, Node child)
{

    if (ln->chLen == ln->chCap)
    {
        ln->chLen *= 2;
        ln->children = (Node*)realloc(ln->children, ln->chLen);
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

    switch (t)
    {
        case NODE_COMPOUND_STMT:
        case NODE_ARGS_LIST:
        case NODE_PARAMS_LIST:
        case NODE_PROGRAMM:
            node.ln = (ListNode*)n;
            break;

        case NODE_VAL:
            node.vn = (ValNode*)n;
            break;
        case NODE_FUNCTION_DECL:
            node.fdn = (FuncDeclNode*)n;
            break;
        case NODE_FUNCTION_PARAM:
            node.fpn = (FuncParamNode*)n;
            break;
        case NODE_FUNCTION_CALL:
            node.fcn = (FuncCallNode*)n;
            break;
        case NODE_VAR_DECL:
            node.vdn = (VarDeclNode*)n;
            break;
        case NODE_BINOP:
            node.bopn = (BinOpNode*)n;
            break;
        case NODE_UNOP:
            node.uopn = (UnOpNode*)n;
            break;
        default:
            break;
    }

    return node;
}
