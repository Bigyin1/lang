#include <stdio.h>
#include <stdlib.h>

#include "ir_comp/compiler.hpp"
#include "optimizer/dag.hpp"
#include "optimizer/dump.hpp"
#include "optimizer/liveness.hpp"
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
    if (argc != 3)
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

    Opt::Liveness la(comp.funcs);

    la.Pass();

    Opt::GraphVizIRDump gw(comp.funcs, la.livenessData);

    gw.genCtx();

    std::ofstream jsonOut(argv[2]);
    gw.Write(jsonOut);

    for (auto&& b : comp.funcs["main"].get()->blocks)
    {
        Opt::DAG::NodeList nodes;

        Opt::DAG::Builder dagBuilder(nodes, b.get(), la.livenessData);
        dagBuilder.BuildDAG();

        Opt::DAG::Optimizer opt(nodes);
        opt.Optimize();

        Opt::DAG::GraphVizDAGDump gwDAG(nodes);

        gwDAG.genCtx();

        std::ofstream dagOut(b.get()->name + ".dot");
        gwDAG.Write(dagOut);
    }

    FreeSemCheck(&ch);
    FreeNode(p.prog);
    DestructLexer(&l);
}
