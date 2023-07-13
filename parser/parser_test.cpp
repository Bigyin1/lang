#include "parser/parser.hpp"

#include <stdio.h>
#include <stdlib.h>

#include "lexer/lexer.hpp"
#include "parser/grVizGen.hpp"

char* readFile(FILE* f)
{

    char* text = NULL;

    if (fseek(f, 0, SEEK_END) != 0)
        return NULL;

    long fSize = ftell(f);
    if (fSize == -1)
        return NULL;

    if (fseek(f, 0, SEEK_SET) != 0)
        return NULL;

    text = (char*)calloc((size_t)fSize + 1, sizeof(char));
    if (text == NULL)
        return NULL;

    if (fread(text, sizeof(char), (size_t)fSize, f) != (size_t)fSize)
    {
        free(text);
        return NULL;
    }

    text[fSize] = '\0';

    fclose(f);
    return text;
}

int main(int argc, char** argv)
{

    if (argc != 3)
        return EXIT_FAILURE;

    FILE* in = fopen(argv[1], "r");
    if (in == NULL)
    {
        perror("parserTest");
        return EXIT_FAILURE;
    }

    FILE* dotOut = fopen(argv[2], "w");

    char* input = readFile(in);
    if (input == NULL)
    {
        perror("parserTest");
        return EXIT_FAILURE;
    }

    Lexer l = NewLexer(input);

    Run(&l);

    if (l.HasErrs)
    {
        printf("failed: ");
        Error(&l.Err, stdout);
        return 1;
    }
    free(input);

    Parser p = NewParser(l.Tokens);

    Run(&p);

    if (p.HasErr)
    {
        PrintParserError(&p.Err, stdout);
        FreeNode(p.prog);
        DestructLexer(&l);

        fclose(dotOut);
        return 1;
    }

    GenGraph(dotOut, p.prog);
    FreeNode(p.prog);

    DestructLexer(&l);

    fclose(dotOut);
}
