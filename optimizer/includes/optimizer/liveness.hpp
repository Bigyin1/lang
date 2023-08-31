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

struct ArgLivenessData
{
    ArgLivenessData() : argLiveState(LivenessState::NotVar), argNextUseInstr(nullptr) {}

    ArgLivenessData(LivenessState _argLiveState, const IR::Instruction* _argNextUseInstr)
        : argLiveState(_argLiveState), argNextUseInstr(_argNextUseInstr)
    {
    }

    bool IsAlive() const
    {
        return this->argLiveState == LivenessState::LiveInBlock ||
               this->argLiveState == LivenessState::LiveOutBlock;
    }

    void SetToDead()
    {
        this->argLiveState    = LivenessState::Dead;
        this->argNextUseInstr = nullptr;
    }

    void SetToLiveInBlock(const IR::Instruction* _argNextUseInstr)
    {
        this->argLiveState    = LivenessState::LiveInBlock;
        this->argNextUseInstr = _argNextUseInstr;
    }

    LivenessState GetState() { return this->argLiveState; }

    const IR::Instruction* GetNextInstr() { return this->argNextUseInstr; }

private:
    LivenessState          argLiveState;
    const IR::Instruction* argNextUseInstr;
};

struct InstrLivenessData
{

    ArgLivenessData GetResLiveness() const { return this->args[0]; }

    ArgLivenessData GetArgLivenessByNum(size_t num) const { return this->args[num]; }

    ArgLivenessData GetBrCondLiveness() const { return this->args[0]; }

    ArgLivenessData GetRetValLiveness() const { return this->args[0]; }

    void AddLivenessData(ArgLivenessData al) { this->args.push_back(al); }

    void SetLivenessToRes(ArgLivenessData al) { this->args[0] = al; }

private:
    std::vector<ArgLivenessData> args = {};
};

typedef std::map<const IR::Instruction*, InstrLivenessData> LivenessData;

class Liveness
{
public:
    Liveness(std::map<IR::FuncName, std::unique_ptr<IR::Function>>& f) : funcs(f), livenessData() {}

    void Pass();

    LivenessData livenessData = {};

private:
    std::map<IR::FuncName, std::unique_ptr<IR::Function>>& funcs;
};

} // namespace Opt
