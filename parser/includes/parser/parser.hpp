/** @file */
#ifndef PARSER_PUBLIC_HPP
#define PARSER_PUBLIC_HPP

#include "parser/errors.hpp"
#include "parser/nodes.hpp"

struct Parser
{
    Token* tokens;
    size_t currTokenIdx;
    Node   prog;

    UnexpTokenError Err;
    bool            HasErr;
};

Parser NewParser(Token* tokens);

void FreeNode(Node n);

void Run(Parser* p);

#endif
