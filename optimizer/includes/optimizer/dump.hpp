/** @file */
#pragma once

#include <inja.hpp>
#include <iostream>
#include <map>

#include "optimizer/dag.hpp"
#include "optimizer/liveness.hpp"

namespace Opt
{

class GraphVizIRDump
{

public:
    GraphVizIRDump(std::map<IR::FuncName, std::unique_ptr<IR::Function>>& f, LivenessData& ld)
        : funcs(f), livenessData(ld)
    {
    }

    void Write(std::ostream& out);

    void genCtx();

private:
    inja::json getInstrObj(IR::Instruction* instr);
    inja::json getBlockNext(IR::BasicBlock* block);

    std::map<IR::FuncName, std::unique_ptr<IR::Function>>& funcs;

    LivenessData& livenessData;

    inja::json ctx;

    std::string tmplPath = "/home/ser/myProjects/lang/optimizer/tmpl/ir/ir.dot.tmpl";
};

namespace DAG
{

class GraphVizDAGDump
{

public:
    GraphVizDAGDump(NodeList& n) : nodes(n) {}

    void Write(std::ostream& out);

    void genCtx();

private:
    inja::json ctx = {};

    NodeList& nodes;

    std::string tmplPath = "/home/ser/myProjects/lang/optimizer/tmpl/dag/dag.dot.tmpl";
};

} // namespace DAG

} // namespace Opt
