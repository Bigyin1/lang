#include <inja.hpp>

#include "optimizer/dump.hpp"

namespace Opt
{

class GWDumpInstrVisitor : public IR::InstrVisitor
{

public:
    GWDumpInstrVisitor(InstrLivenessData&                                     il,
                       std::map<IR::FuncName, std::unique_ptr<IR::Function>>& f)
        : instrLivData(il), funcs(f)
    {
    }

    virtual void visitThreeAddr(const IR::ThreeAddrInstr& instr) override
    {

        this->buildInstrPartJson(addrToStr(&instr) + addrToStr(instr.regRes.get()),
                                 instr.regRes.get()->str());

        this->buildInstrPartJson(addrToStr(&instr), instr.opToStr());

        this->buildInstrPartJson(addrToStr(instr.arg1.get()), instr.arg1.get()->str());

        if (instr.arg2 != nullptr)
            this->buildInstrPartJson(addrToStr(instr.arg2.get()), instr.arg2.get()->str());

        this->buildArgLivenessJson(this->instrLivData.res,
                                   addrToStr(&instr) + addrToStr(instr.regRes.get()));

        this->buildArgLivenessJson(this->instrLivData.args[0], addrToStr(instr.arg1.get()));

        if (instr.arg2 != nullptr)
            this->buildArgLivenessJson(this->instrLivData.args[1], addrToStr(instr.arg2.get()));
    }

    virtual void visitBranchInstr(const IR::BranchInstr& instr) override
    {

        this->buildInstrPartJson(addrToStr(&instr), "br");

        if (instr.cond != nullptr)
            this->buildInstrPartJson(addrToStr(instr.cond.get()), instr.cond.get()->str());

        this->buildInstrPartJson(addrToStr(&instr), instr.ifTrue);

        if (instr.cond != nullptr)
            this->buildInstrPartJson(addrToStr(&instr), instr.ifFalse);

        if (instr.cond != nullptr)
            this->buildArgLivenessJson(this->instrLivData.args[0], addrToStr(instr.cond.get()));
    }

    virtual void visitCallInstr(const IR::CallInstr& instr) override
    {

        // this->buildInstrPartJson(instr.res.get(), instr.res.get()->str());

        // this->buildInstrPartJson(&instr, "call");

        // this->buildInstrPartJson(&instr, "call");
    }

    virtual void visitRetInstr(const IR::RetInstr& instr) override
    {

        this->buildInstrPartJson(addrToStr(&instr), "ret");

        if (instr.retVal != nullptr)
            this->buildInstrPartJson(addrToStr(instr.retVal.get()), instr.retVal.get()->str());
    }

    inja::json instrPartsArr    = nlohmann::json::array();
    inja::json instrLivenessArr = nlohmann::json::array();

private:
    InstrLivenessData& instrLivData;

    std::map<IR::FuncName, std::unique_ptr<IR::Function>>& funcs;

    void buildInstrPartJson(std::string id, std::string str)
    {

        inja::json instrPart;

        instrPart["id"]  = id;
        instrPart["str"] = str;
        this->instrPartsArr.push_back(instrPart);
    }

    void buildArgLivenessJson(InstrLivenessData::ArgLivenessData& argLiv, std::string fromId)
    {
        if (argLiv.first == LivenessState::Dead || argLiv.first == LivenessState::NotVar)
            return;

        const IR::Instruction* next = argLiv.second;

        inja::json instrArgLiveness;

        instrArgLiveness["from"]   = fromId;
        instrArgLiveness["block"]  = this->getInstrBlock(next);
        instrArgLiveness["nextId"] = this->addrToStr(next);
        this->instrLivenessArr.push_back(instrArgLiveness);
    }

    std::string addrToStr(const void* addr)
    {
        std::stringstream ss;

        ss << addr;

        return "x" + ss.str();
    }

    IR::BasicBlockName getInstrBlock(const IR::Instruction* instr)
    {
        for (auto&& func : this->funcs)
        {
            for (auto&& block : func.second.get()->blocks)
            {
                for (auto&& i : block.get()->instrs)
                    if (i.get() == instr)
                        return block.get()->name;
            }
        }

        return "";
    }
};

inja::json GraphVizGen::getInstrObj(IR::Instruction* instr)
{

    inja::json instrJsonObj;

    GWDumpInstrVisitor iv(this->livenessData[instr], this->funcs);

    instr->visit(&iv);

    instrJsonObj["parts"]    = iv.instrPartsArr;
    instrJsonObj["liveness"] = iv.instrLivenessArr;

    return instrJsonObj;
}

inja::json GraphVizGen::getBlockNext(IR::BasicBlock* block)
{
    if (block->instrs.empty())
        return nlohmann::json::array();

    IR::Instruction* lastInstr = block->instrs[block->instrs.size() - 1].get();

    IR::BranchInstr* brInstr = dynamic_cast<IR::BranchInstr*>(lastInstr);
    if (brInstr == nullptr)
        return nlohmann::json::array();

    inja::json nextArr = nlohmann::json::array();

    nextArr.push_back(brInstr->ifTrue);
    if (brInstr->cond != nullptr)
        nextArr.push_back(brInstr->ifFalse);

    return nextArr;
}

void GraphVizGen::genCtx()
{
    inja::json funcsJsonArr = nlohmann::json::array();
    for (auto&& func : this->funcs)
    {

        inja::json blocksJsonArr = nlohmann::json::array();

        for (auto&& block : func.second.get()->blocks)
        {
            inja::json instrsJsonArr = nlohmann::json::array();

            for (auto&& instr : block.get()->instrs)
                instrsJsonArr.push_back(this->getInstrObj(instr.get()));

            inja::json blockJsonObj;
            blockJsonObj["name"]   = block.get()->name;
            blockJsonObj["instrs"] = instrsJsonArr;
            blockJsonObj["next"]   = this->getBlockNext(block.get());

            blocksJsonArr.push_back(blockJsonObj);
        }

        inja::json funcJsonObj;
        funcJsonObj["name"]   = func.first;
        funcJsonObj["blocks"] = blocksJsonArr;

        funcsJsonArr.push_back(funcJsonObj);
    }
    this->ctx["funcs"] = funcsJsonArr;
}

void GraphVizGen::Write(std::ostream& out)
{

    inja::Environment env;

    inja::Template temp   = env.parse_template(this->tmplPath);
    std::string    result = env.render(temp, this->ctx);

    out << result;
}

}; // namespace Opt
