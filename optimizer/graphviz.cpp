#include <inja.hpp>
#include <iterator>

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

    virtual void visitThreeAddr(const IR::ThreeAddrInstr* instr) override
    {

        this->buildInstrPartJson(instr->regRes.get(), instr->regRes.get()->str());

        this->buildInstrPartJson(instr, instr->opToStr(instr->op));

        this->buildInstrPartJson(instr->arg1.get(), instr->arg1.get()->str());

        if (instr->arg2 != nullptr)
            this->buildInstrPartJson(instr->arg2.get(), instr->arg2.get()->str());

        this->buildArgLivenessJson(this->instrLivData.GetResLiveness(), instr->regRes.get());

        this->buildArgLivenessJson(this->instrLivData.GetArgLivenessByNum(1), instr->arg1.get());

        this->buildArgLivenessJson(this->instrLivData.GetArgLivenessByNum(2), instr->arg2.get());
    }

    virtual void visitBranchInstr(const IR::BranchInstr* instr) override
    {

        this->buildInstrPartJson(instr, "br");

        if (instr->cond != nullptr)
            this->buildInstrPartJson(instr->cond.get(), instr->cond.get()->str());

        this->buildInstrPartJson(instr, instr->ifTrue);

        if (instr->cond != nullptr)
            this->buildInstrPartJson(&instr, instr->ifFalse);

        if (instr->cond != nullptr)
            this->buildArgLivenessJson(this->instrLivData.GetResLiveness(), instr->cond.get());
    }

    virtual void visitCallInstr(const IR::CallInstr* instr) override
    {
        this->buildInstrPartJson(instr->res.get(), instr->res.get()->str());
        this->buildArgLivenessJson(this->instrLivData.GetResLiveness(), instr->res.get());

        this->buildInstrPartJson(instr, "call");

        for (size_t i = 0; i < instr->args.size(); i++)
        {
            this->buildInstrPartJson(instr->args[i].get(), instr->args[i].get()->str());
            this->buildArgLivenessJson(this->instrLivData.GetArgLivenessByNum(i + 1),
                                       instr->args[i].get());
        }
    }

    virtual void visitRetInstr(const IR::RetInstr* instr) override
    {

        this->buildInstrPartJson(instr, "ret");

        if (instr->retVal != nullptr)
            this->buildInstrPartJson(instr->retVal.get(), instr->retVal.get()->str());
    }

    inja::json instrPartsArr    = nlohmann::json::array();
    inja::json instrLivenessArr = nlohmann::json::array();

private:
    InstrLivenessData& instrLivData;

    std::map<IR::FuncName, std::unique_ptr<IR::Function>>& funcs;

    void buildInstrPartJson(const void* partId, std::string str)
    {

        inja::json instrPart;

        instrPart["id"]  = this->addrToPartID(partId);
        instrPart["str"] = str;
        this->instrPartsArr.push_back(instrPart);
    }

    void buildArgLivenessJson(ArgLivenessData argLiv, const void* fromId)
    {
        if (!argLiv.IsAlive())
            return;

        const IR::Instruction* next = argLiv.GetNextInstr();

        inja::json instrArgLiveness;

        instrArgLiveness["from"]   = this->addrToPartID(fromId);
        instrArgLiveness["block"]  = this->getInstrBlock(next);
        instrArgLiveness["nextId"] = this->addrToPartID(next);
        this->instrLivenessArr.push_back(instrArgLiveness);
    }

    std::string addrToPartID(const void* addr)
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

inja::json GraphVizIRDump::getInstrObj(IR::Instruction* instr)
{

    inja::json instrJsonObj;

    GWDumpInstrVisitor iv(this->livenessData[instr], this->funcs);

    instr->visit(&iv);

    instrJsonObj["parts"]    = iv.instrPartsArr;
    instrJsonObj["liveness"] = iv.instrLivenessArr;

    return instrJsonObj;
}

inja::json GraphVizIRDump::getBlockNext(IR::BasicBlock* block)
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

void GraphVizIRDump::genCtx()
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

void GraphVizIRDump::Write(std::ostream& out)
{

    inja::Environment env;

    inja::Template temp = env.parse_template(this->tmplPath);

    out << env.render(temp, this->ctx);
}

namespace DAG
{

class GWDumpDAGVisitor : public DAGVisitor
{

public:
    virtual void visitThreeAddr(NodeThreeAddr* n) override
    {
        std::string id = this->getIDFromAddr(n);

        nodeData["id"]    = id;
        nodeData["label"] = IR::ThreeAddrInstr::opToStr(n->GetOp());

        nodeData["to"].push_back(this->getIDFromAddr(n->GetFirstArg().get()));
        if (dynamic_cast<NodeNoOp*>(n->GetSecondArg().get()) == nullptr)
            nodeData["to"].push_back(this->getIDFromAddr(n->GetSecondArg().get()));

        nodeData["boundRegs"] = nlohmann::json::array();

        for (BoundInstrIdx i = 0; i < n->GetBoundInstrsArrRef().size(); i++)
        {
            inja::json boundReg;

            boundReg["id"]   = std::to_string(i);
            boundReg["name"] = n->GetBoundInstrRef(i).resReg.get()->str();
            boundReg["live"] =
                this->livenessToStr(n->GetBoundInstrRef(i).liveness.GetResLiveness().GetState());

            boundReg["to"] = nlohmann::json::array();
            if (this->isThreeAddrOrCall(n->GetFirstArg()))
                boundReg["to"].push_back(this->getBoundRegLocationJson(
                    this->getIDFromAddr(n->GetFirstArg().get()), n->GetBoundInstrRef(i).arg1Idx));

            if (this->isThreeAddrOrCall(n->GetSecondArg()))
                boundReg["to"].push_back(this->getBoundRegLocationJson(
                    this->getIDFromAddr(n->GetSecondArg().get()), n->GetBoundInstrRef(i).arg2Idx));

            nodeData["boundRegs"].push_back(boundReg);
        }
    }

    virtual void visitNodeImm(NodeImm* n) override
    {

        nodeData["id"]    = this->getIDFromAddr(n);
        nodeData["label"] = n->GetVal().str();

        nodeData["to"] = nlohmann::json::array();

        nodeData["boundRegs"] = nlohmann::json::array();
    }

    virtual void visitNodeBranch(NodeBranch* n) override
    {

        nodeData["id"]    = this->getIDFromAddr(n);
        nodeData["label"] = "br";

        nodeData["to"] = nlohmann::json::array();
        if (dynamic_cast<NodeNoOp*>(n->GetCondArg().argDefNode.get()) == nullptr)
        {
            std::string toID = this->getIDFromAddr(n->GetCondArg().argDefNode.get());

            nodeData["to"].push_back(toID);
        }

        nodeData["boundRegs"] = nlohmann::json::array();
    }

    virtual void visitNodeRet(NodeRet* n) override
    {
        nodeData["id"]    = this->getIDFromAddr(n);
        nodeData["label"] = "ret";

        nodeData["to"] = nlohmann::json::array();
        if (dynamic_cast<NodeNoOp*>(n->GetRetArg().argDefNode.get()) == nullptr)
        {
            std::string toID = this->getIDFromAddr(n->GetRetArg().argDefNode.get());

            nodeData["to"].push_back(toID);
        }

        nodeData["boundRegs"] = nlohmann::json::array();
    }

    virtual void visitInitRegVal(NodeInitRegVal* n) override
    {

        nodeData["id"]    = this->getIDFromAddr(n);
        nodeData["label"] = n->GetReg().get()->str();

        nodeData["to"] = nlohmann::json::array();

        nodeData["boundRegs"] = nlohmann::json::array();
    }

    inja::json nodeData = {};

private:
    std::string getIDFromAddr(const void* addr)
    {
        std::stringstream ss;

        ss << "x" << addr;

        return ss.str();
    }

    bool isThreeAddrOrCall(NodePtr n)
    {
        if (n == nullptr)
            return false;

        if (dynamic_cast<const NodeThreeAddr*>(n.get()) == nullptr)
            return false;

        return true;
    }

    std::string livenessToStr(LivenessState ls)
    {

        switch (ls)
        {
            case LivenessState::NotVar:
                return "nv";

            case LivenessState::Dead:
                return "d";

            case LivenessState::LiveInBlock:
                return "lb";

            case LivenessState::LiveOutBlock:
                return "lo";

            default:
                return "";
        }
    }

    inja::json getBoundRegLocationJson(std::string addr, size_t loc)
    {

        return {
            {"node", addr},
            {"idx", std::to_string(loc)},
        };
    }
};

void GraphVizDAGDump::genCtx()
{
    this->ctx["nodes"] = nlohmann::json::array();

    for (auto&& n : this->nodes)
    {
        GWDumpDAGVisitor v;
        n.get()->visit(&v);

        this->ctx["nodes"].push_back(v.nodeData);
    }
}

void GraphVizDAGDump::Write(std::ostream& out)
{
    inja::Environment env;

    inja::Template temp = env.parse_template(this->tmplPath);

    out << env.render(temp, this->ctx);
}

} // namespace DAG

}; // namespace Opt
