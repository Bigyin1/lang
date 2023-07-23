// Code generated DO NOT EDIT

/** @file */
#ifndef LEXEMES_GEN_PUBLIC_HPP
#define LEXEMES_GEN_PUBLIC_HPP


enum TokenName {
	TOK_EOF = 0,

	TOK_FOR = 1ULL << 0,
    TOK_IF = 1ULL << 1,
    TOK_ELSE = 1ULL << 2,
    TOK_VAR = 1ULL << 3,
    TOK_IntType = 1ULL << 4,
    TOK_FloatType = 1ULL << 5,
    TOK_BoolType = 1ULL << 6,
    TOK_TRUE = 1ULL << 7,
    TOK_FALSE = 1ULL << 8,
    TOK_PLUS = 1ULL << 9,
    TOK_MINUS = 1ULL << 10,
    TOK_MULT = 1ULL << 11,
    TOK_DIV = 1ULL << 12,
    TOK_LPAREN = 1ULL << 13,
    TOK_RPAREN = 1ULL << 14,
    TOK_LBRACKET = 1ULL << 15,
    TOK_RBRACKET = 1ULL << 16,
    TOK_LBRACE = 1ULL << 17,
    TOK_RBRACE = 1ULL << 18,
    TOK_ASSIGN = 1ULL << 19,
    TOK_EQ = 1ULL << 20,
    TOK_GE = 1ULL << 21,
    TOK_GT = 1ULL << 22,
    TOK_LE = 1ULL << 23,
    TOK_LT = 1ULL << 24,
    TOK_NE = 1ULL << 25,
    TOK_NOT = 1ULL << 26,
    TOK_LOR = 1ULL << 27,
    TOK_LAND = 1ULL << 28,
    TOK_RETURN = 1ULL << 29,
    TOK_BREAK = 1ULL << 30,
    TOK_CONTINUE = 1ULL << 31,
    TOK_FUNC = 1ULL << 32,
    TOK_COMMA = 1ULL << 33,
    TOK_Integer = 1ULL << 34,
    TOK_Float = 1ULL << 35,
    TOK_ID = 1ULL << 36,
    
};

const TokenName TokenNames[] = {

    TOK_FOR,
    TOK_IF,
    TOK_ELSE,
    TOK_VAR,
    TOK_IntType,
    TOK_FloatType,
    TOK_BoolType,
    TOK_TRUE,
    TOK_FALSE,
    TOK_PLUS,
    TOK_MINUS,
    TOK_MULT,
    TOK_DIV,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACKET,
    TOK_RBRACKET,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_ASSIGN,
    TOK_EQ,
    TOK_GE,
    TOK_GT,
    TOK_LE,
    TOK_LT,
    TOK_NE,
    TOK_NOT,
    TOK_LOR,
    TOK_LAND,
    TOK_RETURN,
    TOK_BREAK,
    TOK_CONTINUE,
    TOK_FUNC,
    TOK_COMMA,
    TOK_Integer,
    TOK_Float,
    TOK_ID,
    

};


TokenName ReservedWordToToken(const char* w);

const char* TokenNameToString(TokenName n);

const char* TokenNameToLexeme(TokenName n);


#endif
