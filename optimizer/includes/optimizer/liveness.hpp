/** @file */
#pragma once

#include <map>

#include "ir_comp/instructions.hpp"
#include "ir_comp/ir.hpp"

namespace Opt
{

enum class LivenessState
{
    NotVar,
    Dead,
    LiveInBlock,
    LiveOutBlock,
};

struct InstrLivenessData
{
    typedef std::pair<LivenessState, const IR::Instruction*> ArgLivenessData;

    std::vector<ArgLivenessData> args;
};

typedef std::map<const IR::Instruction*, InstrLivenessData> LivenessData;

class Liveness
{
public:
    Liveness(std::map<IR::FuncName, std::unique_ptr<IR::Function>>& f) : funcs(f), livenessData() {}

    void Pass();

    LivenessData livenessData;

private:
    std::map<IR::FuncName, std::unique_ptr<IR::Function>>& funcs;
};

} // namespace Opt
