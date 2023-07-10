/** @file */
#ifndef PARSER_PRIVATE_HPP
#define PARSER_PRIVATE_HPP

#include "parser/parser.hpp"

Token* getCurrToken(Parser* p);

bool currTokenHasName(Parser* p, TokenNamesSet n);

void eatToken(Parser* p, TokenNamesSet exp);

TokenName peek(Parser* n);

#endif
