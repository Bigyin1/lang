// Code generated DO NOT EDIT

/** @file */
#ifndef LEXEMES_GEN_PUBLIC_HPP
#define LEXEMES_GEN_PUBLIC_HPP


enum TokenName {
	TokEOF = 0,

	For,
    While,
    If,
    Else,
    IntType,
    FloatType,
    Plus,
    Minus,
    Mult,
    Div,
    LParen,
    RParen,
    LSParen,
    RSParen,
    LCParen,
    RCParen,
    Assign,
    EQ,
    GE,
    GT,
    LE,
    LT,
    NE,
    Integer,
    Float,
    ID,
    
};

TokenName ReservedWordToToken(const char* w);

const char* TokenNameToString(TokenName n);


#endif
