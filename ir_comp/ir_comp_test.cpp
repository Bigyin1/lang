#include <stdio.h>
#include <stdlib.h>

#include "ir_comp/compiler.hpp"
#include "parser/parser.hpp"
#include "semantic/checker.hpp"

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
    if (argc != 2)
        return EXIT_FAILURE;

    FILE* in = fopen(argv[1], "r");
    if (in == NULL)
        return EXIT_FAILURE;

    char* input = readFile(in);
    if (input == NULL)
        return EXIT_FAILURE;

    Lexer l = NewLexer(input);

    Run(&l);
    if (l.HasErrs)
    {
        printf("failed: ");
        Error(&l.Err, stdout);
        return EXIT_FAILURE;
    }
    free(input);

    Parser p = NewParser(l.Tokens);

    Run(&p);
    if (p.HasErr)
    {
        PrintParserError(&p.Err, stdout);
        return EXIT_FAILURE;
    }

    SemanticChecker ch = {};

    RunSemCheck(&ch, p.prog.ln);

    if (ch.errsSz != 0)
    {
        for (size_t i = 0; i < ch.errsSz; i++)
            PrintError(&ch.errors[i], stdout);

        return EXIT_FAILURE;
    }

    IRCompiler comp(p.prog, ch.rootScope);

    comp.RunAstToIRCompiler();

    comp.Print(std::cout);

    FreeSemCheck(&ch);
    FreeNode(p.prog);
    DestructLexer(&l);
}
