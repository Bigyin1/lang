/** @file */
#ifndef PARSER_NODES_PRIVATE_HPP
#define PARSER_NODES_PRIVATE_HPP

#include "parser/parser.hpp"

Node NewListNode(NodeType nt);

void ListNodeAddChild(ListNode* ln, Node child);

Node NewBinOpNode(Node l, Node r, Token* op);

Node NewUnOpNode(Node e, Token* op);

Node NewValNode(Token* val);

Node NewVarDeclNode(Token* name, Token* type, Node init);

Node NewIfStmtNode(Token* op, Node cond, ListNode* body, Node elseBody);

Node NewForStmtNode(Token* op, Node cond, ListNode* body);

Node NewFuncDeclNode(Token* name, ListNode* params, Token* ret, ListNode* body);

Node NewNodeWithType(void* n, NodeType t);

#endif
