/** @file */
#ifndef PARSER_NODES_PUBLIC_HPP
#define PARSER_NODES_PUBLIC_HPP

#include "lexer/lexer.hpp"

enum NodeType
{
    NODE_EMTY = 0,

    NODE_PROGRAMM,
    NODE_COMPOUND_STMT,
    NODE_PARAMS_LIST,
    NODE_ARGS_LIST,

    NODE_FUNCTION_DECL,
    NODE_FUNCTION_PARAM,
    NODE_FUNCTION_CALL,
    NODE_VAR_DECL,
    NODE_VAL,
    NODE_BINOP,
    NODE_UNOP,
};

struct ListNode;
struct FuncCallNode;
struct FuncDeclNode;
struct FuncParamNode;
struct VarDeclNode;
struct ValNode;
struct BinOpNode;
struct UnOpNode;

struct NodeHeader
{
    NodeType type;
};

struct Node
{
    NodeHeader hdr;
    union
    {
        ListNode*      ln;
        ValNode*       vn;
        VarDeclNode*   vdn;
        FuncParamNode* fpn;
        FuncDeclNode*  fdn;
        FuncCallNode*  fcn;
        BinOpNode*     bopn;
        UnOpNode*      uopn;
    };
};

struct ListNode
{
    Node*  children;
    size_t chLen;
    size_t chCap;
};

struct ValNode
{
    Token* val;
};

struct VarDeclNode
{
    Token* varName;
    Token* typeName;
    Node   initVal;
};

struct FuncParamNode
{
    Token* paramName;
    Token* typeName;
};

struct FuncDeclNode
{
    Token* fName;

    ListNode* params;

    Token* retTypeName;

    ListNode* body;
};

struct FuncCallNode
{
    Token* fName;

    ListNode* args;
};

struct BinOpNode
{
    Token* op;

    Node left;
    Node right;
};

struct UnOpNode
{
    Token* op;

    Node child;
};

#endif
