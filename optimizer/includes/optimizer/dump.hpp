/** @file */
#pragma once

#include <inja.hpp>
#include <iostream>
#include <map>

#include "optimizer/liveness.hpp"

namespace Opt
{

class GraphVizGen
{

public:
    GraphVizGen(std::map<IR::FuncName, std::unique_ptr<IR::Function>>& f, LivenessData& ld)
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

    std::string tmplPath = "/home/ser/myProjects/lang/optimizer/tmpl/ir.dot.tmpl";
};

} // namespace Opt
