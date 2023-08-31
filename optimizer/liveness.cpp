#include "optimizer/liveness.hpp"

#include <inja.hpp>

namespace Opt
{

typedef std::map<const IR::Register*, const IR::Instruction*> NextRegUseTab;

class ArgLivnessVisitor : public IR::ArgVisitor
{
private:
    void modifyLivenessTable(const IR::Register* reg)
    {
        this->tab[reg] = livenessData.GetNextInstr();
    }

    ArgLivenessData collectRegLivenessData(const IR::Register* reg)
    {
        const IR::Instruction* nextUse = this->tab[reg];

        LivenessState st;
        if (nextUse == nullptr)
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

        return ArgLivenessData(st, nextUse);
    }

public:
    ArgLivnessVisitor(Opt::NextRegUseTab& t) : tab(t) {}

    virtual void visitRegArg(const IR::RegArg* rarg) override
    {
        if (this->livenessData.GetState() != LivenessState::NotVar)
        {
            this->modifyLivenessTable(rarg->GetReg().get());
            return;
        }

        this->livenessData = this->collectRegLivenessData(rarg->GetReg().get());
    }

    virtual void visitImmArg(const IR::ImmArg*) override {}

    Opt::NextRegUseTab&   tab;
    const IR::BasicBlock* currBlock = nullptr;

    ArgLivenessData livenessData = {};
};

class InstrLivenessVisitor : public IR::InstrVisitor
{

private:
    void getCurrArgLivness(IR::InstrArg* arg)
    {

        if (arg == nullptr)
        {
            this->lw.AddLivenessData(ArgLivenessData{});
            return;
        }

        this->argVisitor.livenessData = {};
        arg->visit(&this->argVisitor);

        this->lw.AddLivenessData(this->argVisitor.livenessData);
    }

    void setResToDead(IR::RegArg* resReg)
    {
        if (resReg->GetType() == IR::DataType::Void)
            return;

        this->argVisitor.livenessData.SetToDead();

        this->argVisitor.visitRegArg(resReg);
    }

    void setArgToLive(IR::InstrArg* arg, const IR::Instruction* instr)
    {
        if (arg == nullptr)
            return;

        this->argVisitor.livenessData.SetToLiveInBlock(instr);

        arg->visit(&this->argVisitor);
    }

public:
    InstrLivenessVisitor(Opt::NextRegUseTab& t) : argVisitor(t) {}

    virtual void visitThreeAddr(const IR::ThreeAddrInstr* instr) override
    {
        this->getCurrArgLivness(instr->regRes.get());
        this->getCurrArgLivness(instr->arg1.get());
        this->getCurrArgLivness(instr->arg2.get());

        this->setResToDead(instr->regRes.get());
        this->setArgToLive(instr->arg1.get(), instr);
        this->setArgToLive(instr->arg2.get(), instr);
    }

    virtual void visitBranchInstr(const IR::BranchInstr* instr) override
    {
        this->getCurrArgLivness(instr->cond.get());

        this->setArgToLive(instr->cond.get(), instr);
    }

    virtual void visitCallInstr(const IR::CallInstr* instr) override
    {
        this->getCurrArgLivness(instr->res.get());
        for (auto&& arg : instr->args)
            this->getCurrArgLivness(arg.get());

        this->setResToDead(instr->res.get());
        for (auto&& arg : instr->args)
            this->setArgToLive(arg.get(), instr);
    }

    virtual void visitRetInstr(const IR::RetInstr* instr) override
    {
        this->getCurrArgLivness(instr->retVal.get());

        this->setArgToLive(instr->retVal.get(), instr);
    }

    void SetCurrBlock(const IR::BasicBlock* currBlock) { this->argVisitor.currBlock = currBlock; }

    Opt::InstrLivenessData GetNewLivenessData(IR::Instruction* instr)
    {
        this->lw = {};

        instr->visit(this);

        return this->lw;
    }

    Opt::InstrLivenessData lw = {};

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
