// Code generated DO NOT EDIT

/** @file */
#ifndef LEXEMES_GEN_PUBLIC_HPP
#define LEXEMES_GEN_PUBLIC_HPP


enum TokenName {
	TokEOF = 0,

	FOR = 1ULL << 0,
    IF = 1ULL << 1,
    ELSE = 1ULL << 2,
    VAR = 1ULL << 3,
    IntType = 1ULL << 4,
    FloatType = 1ULL << 5,
    BoolType = 1ULL << 6,
    TRUE = 1ULL << 7,
    FALSE = 1ULL << 8,
    PLUS = 1ULL << 9,
    MINUS = 1ULL << 10,
    MULT = 1ULL << 11,
    DIV = 1ULL << 12,
    LPAREN = 1ULL << 13,
    RPAREN = 1ULL << 14,
    LBRACKET = 1ULL << 15,
    RBRACKET = 1ULL << 16,
    LBRACE = 1ULL << 17,
    RBRACE = 1ULL << 18,
    ASSIGN = 1ULL << 19,
    EQ = 1ULL << 20,
    GE = 1ULL << 21,
    GT = 1ULL << 22,
    LE = 1ULL << 23,
    LT = 1ULL << 24,
    NE = 1ULL << 25,
    RETURN = 1ULL << 26,
    BREAK = 1ULL << 27,
    CONTINUE = 1ULL << 28,
    FUNC = 1ULL << 29,
    COMMA = 1ULL << 30,
    Integer = 1ULL << 31,
    Float = 1ULL << 32,
    ID = 1ULL << 33,
    
};

TokenName ReservedWordToToken(const char* w);

const char* TokenNameToString(TokenName n);


#endif
