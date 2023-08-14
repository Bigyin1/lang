#include "optimizer/liveness.hpp"

#include <inja.hpp>

namespace Opt
{

typedef std::map<const IR::Register*, const IR::Instruction*> NextRegUseTab;

class ArgLivnessVisitor : public IR::ArgVisitor
{
private:
    void modifyLivenessTable(const IR::Register* reg) { tab[reg] = livenessData.second; }

    InstrLivenessData::ArgLivenessData collectRegLivenessData(const IR::Register* reg)
    {
        const IR::Instruction* nextUse = tab[reg];

        LivenessState st;
        if (nextUse == NULL)
        {
            st = LivenessState::Dead;
        }
        else
        {
            st = LivenessState::LiveOutBlock;
            for (auto&& instr : this->currBlock->instrs)
            {
                if (instr.get() == nextUse)
                    st = LivenessState::LiveInBlock;
            }
        }

        return InstrLivenessData::ArgLivenessData(st, nextUse);
    }

public:
    ArgLivnessVisitor(Opt::NextRegUseTab& t) : tab(t) {}

    void visitReg(const IR::Register* reg)
    {
        if (this->livenessData.first != LivenessState::NotVar)
        {
            this->modifyLivenessTable(reg);
            return;
        }

        this->livenessData = this->collectRegLivenessData(reg);
    }

    virtual void visitRegArg(const IR::RegArg& rarg) override { visitReg(rarg.GetReg().get()); }

    virtual void visitImmArg(const IR::ImmArg&) override {}

    Opt::NextRegUseTab&   tab;
    const IR::BasicBlock* currBlock;

    InstrLivenessData::ArgLivenessData livenessData;
};

class InstrLivenessVisitor : public IR::InstrVisitor
{

private:
    void getResLivness(const IR::Register* resReg)
    {
        if (resReg == nullptr)
            return;

        this->argVisitor.livenessData = {};

        argVisitor.visitReg(resReg);

        this->lw.res = this->argVisitor.livenessData;
    }

    void getArgLivness(IR::InstrArg* arg)
    {
        if (arg == nullptr)
            return;

        this->argVisitor.livenessData = {};

        arg->visit(&this->argVisitor);

        this->lw.args.push_back(this->argVisitor.livenessData);
    }

    void setResToDead(const IR::Register* resReg)
    {
        if (resReg == nullptr)
            return;

        this->argVisitor.livenessData.first  = LivenessState::Dead;
        this->argVisitor.livenessData.second = nullptr;

        this->argVisitor.visitReg(resReg);
    }

    void setArgToLive(IR::InstrArg* arg, const IR::Instruction* instr)
    {
        if (arg == nullptr)
            return;

        this->argVisitor.livenessData.first  = LivenessState::LiveInBlock;
        this->argVisitor.livenessData.second = instr;

        arg->visit(&this->argVisitor);
    }

public:
    InstrLivenessVisitor(Opt::NextRegUseTab& t) : argVisitor(t) {}

    virtual void visitThreeAddr(const IR::ThreeAddrInstr& instr) override
    {
        this->getResLivness(instr.regRes.get());
        this->getArgLivness(instr.arg1.get());
        this->getArgLivness(instr.arg2.get());

        this->setResToDead(instr.regRes.get());
        this->setArgToLive(instr.arg1.get(), &instr);
        this->setArgToLive(instr.arg2.get(), &instr);
    }

    virtual void visitBranchInstr(const IR::BranchInstr& instr) override
    {
        this->getArgLivness(instr.cond.get());

        this->setArgToLive(instr.cond.get(), &instr);
    }

    virtual void visitCallInstr(const IR::CallInstr& instr) override
    {
        this->getResLivness(instr.res.get());

        for (auto&& arg : instr.args)
            this->getArgLivness(arg.get());

        for (auto&& arg : instr.args)
            this->setArgToLive(arg.get(), &instr);
    }

    virtual void visitRetInstr(const IR::RetInstr& instr) override
    {
        this->getArgLivness(instr.retVal.get());

        this->setArgToLive(instr.retVal.get(), &instr);
    }

    void SetCurrBlock(const IR::BasicBlock* currBlock) { this->argVisitor.currBlock = currBlock; }

    Opt::InstrLivenessData GetNewLivenessData(IR::Instruction* instr)
    {
        this->lw = {};

        instr->visit(this);

        return this->lw;
    }

    Opt::InstrLivenessData lw;

    ArgLivnessVisitor argVisitor;
};

void Opt::Liveness::Pass()
{
    NextRegUseTab tab;

    InstrLivenessVisitor lv(tab);

    for (auto&& func : this->funcs)
    {
        tab.clear();
        tab[nullptr] = nullptr;

        auto& blocks = func.second.get()->blocks;

        for (auto bl = blocks.rbegin(); bl != blocks.rend(); ++bl)
        {
            lv.SetCurrBlock((*bl).get());
            auto& instrs = (*bl).get()->instrs;

            for (auto ins = instrs.rbegin(); ins != instrs.rend(); ++ins)
            {
                IR::Instruction* currInstr = (*ins).get();

                this->livenessData[currInstr] = lv.GetNewLivenessData(currInstr);
            }
        }
    }
}

} // namespace Opt
