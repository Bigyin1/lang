// Code generated DO NOT EDIT
#include "lexer/lexemes.hpp"

#include <string.h>
#include <stdlib.h>


TokenName ReservedWordToToken(const char* w) {

     
    if (strcmp(w, "for") == 0)
        return FOR;
     
    if (strcmp(w, "if") == 0)
        return IF;
     
    if (strcmp(w, "else") == 0)
        return ELSE;
     
    if (strcmp(w, "var") == 0)
        return VAR;
     
    if (strcmp(w, "int") == 0)
        return IntType;
     
    if (strcmp(w, "float") == 0)
        return FloatType;
     
    if (strcmp(w, "bool") == 0)
        return BoolType;
     
    if (strcmp(w, "true") == 0)
        return TRUE;
     
    if (strcmp(w, "false") == 0)
        return FALSE;
     
    if (strcmp(w, "+") == 0)
        return PLUS;
     
    if (strcmp(w, "-") == 0)
        return MINUS;
     
    if (strcmp(w, "*") == 0)
        return MULT;
     
    if (strcmp(w, "/") == 0)
        return DIV;
     
    if (strcmp(w, "(") == 0)
        return LPAREN;
     
    if (strcmp(w, ")") == 0)
        return RPAREN;
     
    if (strcmp(w, "[") == 0)
        return LBRACKET;
     
    if (strcmp(w, "]") == 0)
        return RBRACKET;
     
    if (strcmp(w, "{") == 0)
        return LBRACE;
     
    if (strcmp(w, "}") == 0)
        return RBRACE;
     
    if (strcmp(w, "=") == 0)
        return ASSIGN;
     
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
     
    if (strcmp(w, "return") == 0)
        return RETURN;
     
    if (strcmp(w, "break") == 0)
        return BREAK;
     
    if (strcmp(w, "continue") == 0)
        return CONTINUE;
     
    if (strcmp(w, "func") == 0)
        return FUNC;
     
    if (strcmp(w, ",") == 0)
        return COMMA;
    
    return TokEOF;
}

const char* TokenNameToString(TokenName n) {
    
    if (FOR == n) {
        return "FOR";
    }
    
    if (IF == n) {
        return "IF";
    }
    
    if (ELSE == n) {
        return "ELSE";
    }
    
    if (VAR == n) {
        return "VAR";
    }
    
    if (IntType == n) {
        return "IntType";
    }
    
    if (FloatType == n) {
        return "FloatType";
    }
    
    if (BoolType == n) {
        return "BoolType";
    }
    
    if (TRUE == n) {
        return "TRUE";
    }
    
    if (FALSE == n) {
        return "FALSE";
    }
    
    if (PLUS == n) {
        return "PLUS";
    }
    
    if (MINUS == n) {
        return "MINUS";
    }
    
    if (MULT == n) {
        return "MULT";
    }
    
    if (DIV == n) {
        return "DIV";
    }
    
    if (LPAREN == n) {
        return "LPAREN";
    }
    
    if (RPAREN == n) {
        return "RPAREN";
    }
    
    if (LBRACKET == n) {
        return "LBRACKET";
    }
    
    if (RBRACKET == n) {
        return "RBRACKET";
    }
    
    if (LBRACE == n) {
        return "LBRACE";
    }
    
    if (RBRACE == n) {
        return "RBRACE";
    }
    
    if (ASSIGN == n) {
        return "ASSIGN";
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
    
    if (RETURN == n) {
        return "RETURN";
    }
    
    if (BREAK == n) {
        return "BREAK";
    }
    
    if (CONTINUE == n) {
        return "CONTINUE";
    }
    
    if (FUNC == n) {
        return "FUNC";
    }
    
    if (COMMA == n) {
        return "COMMA";
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



