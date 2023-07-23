// Code generated DO NOT EDIT
#include "lexer/lexemes.hpp"

#include <string.h>
#include <stdlib.h>


TokenName ReservedWordToToken(const char* w) {

     
    if (strcmp(w, "for") == 0)
        return TOK_FOR;
     
    if (strcmp(w, "if") == 0)
        return TOK_IF;
     
    if (strcmp(w, "else") == 0)
        return TOK_ELSE;
     
    if (strcmp(w, "var") == 0)
        return TOK_VAR;
     
    if (strcmp(w, "int") == 0)
        return TOK_IntType;
     
    if (strcmp(w, "float") == 0)
        return TOK_FloatType;
     
    if (strcmp(w, "bool") == 0)
        return TOK_BoolType;
     
    if (strcmp(w, "true") == 0)
        return TOK_TRUE;
     
    if (strcmp(w, "false") == 0)
        return TOK_FALSE;
     
    if (strcmp(w, "+") == 0)
        return TOK_PLUS;
     
    if (strcmp(w, "-") == 0)
        return TOK_MINUS;
     
    if (strcmp(w, "*") == 0)
        return TOK_MULT;
     
    if (strcmp(w, "/") == 0)
        return TOK_DIV;
     
    if (strcmp(w, "(") == 0)
        return TOK_LPAREN;
     
    if (strcmp(w, ")") == 0)
        return TOK_RPAREN;
     
    if (strcmp(w, "[") == 0)
        return TOK_LBRACKET;
     
    if (strcmp(w, "]") == 0)
        return TOK_RBRACKET;
     
    if (strcmp(w, "{") == 0)
        return TOK_LBRACE;
     
    if (strcmp(w, "}") == 0)
        return TOK_RBRACE;
     
    if (strcmp(w, "=") == 0)
        return TOK_ASSIGN;
     
    if (strcmp(w, "==") == 0)
        return TOK_EQ;
     
    if (strcmp(w, ">=") == 0)
        return TOK_GE;
     
    if (strcmp(w, ">") == 0)
        return TOK_GT;
     
    if (strcmp(w, "<=") == 0)
        return TOK_LE;
     
    if (strcmp(w, "<") == 0)
        return TOK_LT;
     
    if (strcmp(w, "!=") == 0)
        return TOK_NE;
     
    if (strcmp(w, "!") == 0)
        return TOK_NOT;
     
    if (strcmp(w, "||") == 0)
        return TOK_LOR;
     
    if (strcmp(w, "&&") == 0)
        return TOK_LAND;
     
    if (strcmp(w, "return") == 0)
        return TOK_RETURN;
     
    if (strcmp(w, "break") == 0)
        return TOK_BREAK;
     
    if (strcmp(w, "continue") == 0)
        return TOK_CONTINUE;
     
    if (strcmp(w, "func") == 0)
        return TOK_FUNC;
     
    if (strcmp(w, ",") == 0)
        return TOK_COMMA;
    
    return TOK_EOF;
}

const char* TokenNameToString(TokenName n) {
    
    if (TOK_FOR == n) {
        return "FOR";
    }
    
    if (TOK_IF == n) {
        return "IF";
    }
    
    if (TOK_ELSE == n) {
        return "ELSE";
    }
    
    if (TOK_VAR == n) {
        return "VAR";
    }
    
    if (TOK_IntType == n) {
        return "IntType";
    }
    
    if (TOK_FloatType == n) {
        return "FloatType";
    }
    
    if (TOK_BoolType == n) {
        return "BoolType";
    }
    
    if (TOK_TRUE == n) {
        return "TRUE";
    }
    
    if (TOK_FALSE == n) {
        return "FALSE";
    }
    
    if (TOK_PLUS == n) {
        return "PLUS";
    }
    
    if (TOK_MINUS == n) {
        return "MINUS";
    }
    
    if (TOK_MULT == n) {
        return "MULT";
    }
    
    if (TOK_DIV == n) {
        return "DIV";
    }
    
    if (TOK_LPAREN == n) {
        return "LPAREN";
    }
    
    if (TOK_RPAREN == n) {
        return "RPAREN";
    }
    
    if (TOK_LBRACKET == n) {
        return "LBRACKET";
    }
    
    if (TOK_RBRACKET == n) {
        return "RBRACKET";
    }
    
    if (TOK_LBRACE == n) {
        return "LBRACE";
    }
    
    if (TOK_RBRACE == n) {
        return "RBRACE";
    }
    
    if (TOK_ASSIGN == n) {
        return "ASSIGN";
    }
    
    if (TOK_EQ == n) {
        return "EQ";
    }
    
    if (TOK_GE == n) {
        return "GE";
    }
    
    if (TOK_GT == n) {
        return "GT";
    }
    
    if (TOK_LE == n) {
        return "LE";
    }
    
    if (TOK_LT == n) {
        return "LT";
    }
    
    if (TOK_NE == n) {
        return "NE";
    }
    
    if (TOK_NOT == n) {
        return "NOT";
    }
    
    if (TOK_LOR == n) {
        return "LOR";
    }
    
    if (TOK_LAND == n) {
        return "LAND";
    }
    
    if (TOK_RETURN == n) {
        return "RETURN";
    }
    
    if (TOK_BREAK == n) {
        return "BREAK";
    }
    
    if (TOK_CONTINUE == n) {
        return "CONTINUE";
    }
    
    if (TOK_FUNC == n) {
        return "FUNC";
    }
    
    if (TOK_COMMA == n) {
        return "COMMA";
    }
    

    
    if (TOK_Integer == n) {
        return "Integer";
    }
    
    if (TOK_Float == n) {
        return "Float";
    }
    
    if (TOK_ID == n) {
        return "ID";
    }
    

    if (TOK_EOF == n) {
            return "EOF";
    }

    exit(1);
}

const char* TokenNameToLexeme(TokenName n) {
    
    if (TOK_FOR == n) {
        return "for";
    }
    
    if (TOK_IF == n) {
        return "if";
    }
    
    if (TOK_ELSE == n) {
        return "else";
    }
    
    if (TOK_VAR == n) {
        return "var";
    }
    
    if (TOK_IntType == n) {
        return "int";
    }
    
    if (TOK_FloatType == n) {
        return "float";
    }
    
    if (TOK_BoolType == n) {
        return "bool";
    }
    
    if (TOK_TRUE == n) {
        return "true";
    }
    
    if (TOK_FALSE == n) {
        return "false";
    }
    
    if (TOK_PLUS == n) {
        return "+";
    }
    
    if (TOK_MINUS == n) {
        return "-";
    }
    
    if (TOK_MULT == n) {
        return "*";
    }
    
    if (TOK_DIV == n) {
        return "/";
    }
    
    if (TOK_LPAREN == n) {
        return "(";
    }
    
    if (TOK_RPAREN == n) {
        return ")";
    }
    
    if (TOK_LBRACKET == n) {
        return "[";
    }
    
    if (TOK_RBRACKET == n) {
        return "]";
    }
    
    if (TOK_LBRACE == n) {
        return "{";
    }
    
    if (TOK_RBRACE == n) {
        return "}";
    }
    
    if (TOK_ASSIGN == n) {
        return "=";
    }
    
    if (TOK_EQ == n) {
        return "==";
    }
    
    if (TOK_GE == n) {
        return ">=";
    }
    
    if (TOK_GT == n) {
        return ">";
    }
    
    if (TOK_LE == n) {
        return "<=";
    }
    
    if (TOK_LT == n) {
        return "<";
    }
    
    if (TOK_NE == n) {
        return "!=";
    }
    
    if (TOK_NOT == n) {
        return "!";
    }
    
    if (TOK_LOR == n) {
        return "||";
    }
    
    if (TOK_LAND == n) {
        return "&&";
    }
    
    if (TOK_RETURN == n) {
        return "return";
    }
    
    if (TOK_BREAK == n) {
        return "break";
    }
    
    if (TOK_CONTINUE == n) {
        return "continue";
    }
    
    if (TOK_FUNC == n) {
        return "func";
    }
    
    if (TOK_COMMA == n) {
        return ",";
    }
    

    
    if (TOK_Integer == n) {
        return "";
    }
    
    if (TOK_Float == n) {
        return "";
    }
    
    if (TOK_ID == n) {
        return "";
    }
    

    if (TOK_EOF == n) {
            return "";
    }

    exit(1);
}



