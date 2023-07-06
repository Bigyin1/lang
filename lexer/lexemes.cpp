// Code generated DO NOT EDIT
#include "lexer/lexemes.hpp"

#include <string.h>
#include <stdlib.h>


TokenName ReservedWordToToken(const char* w) {

     
    if (strcmp(w, "for") == 0)
        return For;
     
    if (strcmp(w, "while") == 0)
        return While;
     
    if (strcmp(w, "if") == 0)
        return If;
     
    if (strcmp(w, "else") == 0)
        return Else;
     
    if (strcmp(w, "int") == 0)
        return IntType;
     
    if (strcmp(w, "float") == 0)
        return FloatType;
     
    if (strcmp(w, "+") == 0)
        return Plus;
     
    if (strcmp(w, "-") == 0)
        return Minus;
     
    if (strcmp(w, "*") == 0)
        return Mult;
     
    if (strcmp(w, "/") == 0)
        return Div;
     
    if (strcmp(w, "(") == 0)
        return LParen;
     
    if (strcmp(w, ")") == 0)
        return RParen;
     
    if (strcmp(w, "[") == 0)
        return LSParen;
     
    if (strcmp(w, "]") == 0)
        return RSParen;
     
    if (strcmp(w, "{") == 0)
        return LCParen;
     
    if (strcmp(w, "}") == 0)
        return RCParen;
     
    if (strcmp(w, "=") == 0)
        return Assign;
     
    if (strcmp(w, "==") == 0)
        return EQ;
     
    if (strcmp(w, ">=") == 0)
        return GE;
     
    if (strcmp(w, ">") == 0)
        return GT;
     
    if (strcmp(w, "<=") == 0)
        return LE;
     
    if (strcmp(w, "<") == 0)
        return LT;
     
    if (strcmp(w, "!=") == 0)
        return NE;
    
    return TokEOF;
}

const char* TokenNameToString(TokenName n) {
    
    if (For == n) {
        return "For";
    }
    
    if (While == n) {
        return "While";
    }
    
    if (If == n) {
        return "If";
    }
    
    if (Else == n) {
        return "Else";
    }
    
    if (IntType == n) {
        return "IntType";
    }
    
    if (FloatType == n) {
        return "FloatType";
    }
    
    if (Plus == n) {
        return "Plus";
    }
    
    if (Minus == n) {
        return "Minus";
    }
    
    if (Mult == n) {
        return "Mult";
    }
    
    if (Div == n) {
        return "Div";
    }
    
    if (LParen == n) {
        return "LParen";
    }
    
    if (RParen == n) {
        return "RParen";
    }
    
    if (LSParen == n) {
        return "LSParen";
    }
    
    if (RSParen == n) {
        return "RSParen";
    }
    
    if (LCParen == n) {
        return "LCParen";
    }
    
    if (RCParen == n) {
        return "RCParen";
    }
    
    if (Assign == n) {
        return "Assign";
    }
    
    if (EQ == n) {
        return "EQ";
    }
    
    if (GE == n) {
        return "GE";
    }
    
    if (GT == n) {
        return "GT";
    }
    
    if (LE == n) {
        return "LE";
    }
    
    if (LT == n) {
        return "LT";
    }
    
    if (NE == n) {
        return "NE";
    }
    

    
    if (Integer == n) {
        return "Integer";
    }
    
    if (Float == n) {
        return "Float";
    }
    
    if (ID == n) {
        return "ID";
    }
    

    if (TokEOF == n) {
            return "EOF";
    }

    exit(1);
}



