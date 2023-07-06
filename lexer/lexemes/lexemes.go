package main

type LexemeMeta struct {
	Lexeme    string
	TokenName string
}

var ConstantLexemes = []LexemeMeta{

	{
		Lexeme:    "for",
		TokenName: "FOR",
	},
	{
		Lexeme:    "while",
		TokenName: "WHILE",
	},
	{
		Lexeme:    "if",
		TokenName: "IF",
	},
	{
		Lexeme:    "else",
		TokenName: "ELSE",
	},
	{
		Lexeme:    "int",
		TokenName: "IntType",
	},
	{
		Lexeme:    "float",
		TokenName: "FloatType",
	},
	{
		Lexeme:    "+",
		TokenName: "PLUS",
	},
	{
		Lexeme:    "-",
		TokenName: "MINUS",
	},
	{
		Lexeme:    "*",
		TokenName: "MULT",
	},
	{
		Lexeme:    "/",
		TokenName: "DIV",
	},
	{
		Lexeme:    "(",
		TokenName: "LPAREN",
	},
	{
		Lexeme:    ")",
		TokenName: "RPAREN",
	},
	{
		Lexeme:    "[",
		TokenName: "LBRACKET",
	},
	{
		Lexeme:    "]",
		TokenName: "RBRACKET",
	},
	{
		Lexeme:    "{",
		TokenName: "LBRACE",
	},
	{
		Lexeme:    "}",
		TokenName: "RBRACE",
	},
	{
		Lexeme:    "=",
		TokenName: "ASSIGN",
	},
	{
		Lexeme:    "==",
		TokenName: "EQ",
	},
	{
		Lexeme:    ">=",
		TokenName: "GE",
	},
	{
		Lexeme:    ">",
		TokenName: "GT",
	},
	{
		Lexeme:    "<=",
		TokenName: "LE",
	},
	{
		Lexeme:    "<",
		TokenName: "LT",
	},
	{
		Lexeme:    "!=",
		TokenName: "NE",
	},
	{
		Lexeme:    "return",
		TokenName: "RET",
	},
	{
		Lexeme:    "func",
		TokenName: "FUNC",
	},
	{
		Lexeme:    ",",
		TokenName: "COMMA",
	},
}

var VariableLexemes = []LexemeMeta{
	{
		TokenName: "Integer",
	},
	{
		TokenName: "Float",
	},
	{
		TokenName: "ID",
	},
}
