#include <stdexcept>

#include "optimizer/dag.hpp"

namespace Opt
{
namespace DAG
{

class FoldOptDAGVisitor : public DAGVisitor
{
public:
    virtual void visitThreeAddr(NodeThreeAddr* n) override
    {

        IR::ImmArg tryArg1 = n->GetFirstArg().get()->GetComputedVal();
        IR::ImmArg tryArg2 = n->GetSecondArg().get()->GetComputedVal();

        if (!tryArg1.HasType() && !tryArg2.HasType())
            return;

        IR::ImmArg res;
        switch (n->GetOp())
        {
            case IR::ThreeAddrInstr::Opcode::STORE:
                res = tryArg1;
                break;

            case IR::ThreeAddrInstr::Opcode::ADD:
                res = this->compADD(tryArg1, tryArg2);
                break;

            case IR::ThreeAddrInstr::Opcode::SUB:
                res = this->compSUB(tryArg1, tryArg2);
                break;

            case IR::ThreeAddrInstr::Opcode::MUL:
                res = this->compMULT(tryArg1, tryArg2);
                break;

            case IR::ThreeAddrInstr::Opcode::DIV:
                res = this->compDIV(tryArg1, tryArg2);
                break;

            case IR::ThreeAddrInstr::Opcode::NEG:
                res = this->compNEG(tryArg1);
                break;

            case IR::ThreeAddrInstr::Opcode::NEQ:
                res = this->compNEQ(tryArg1, tryArg2);
                break;

            case IR::ThreeAddrInstr::Opcode::EQ:
                res = this->compEQ(tryArg1, tryArg2);
                break;

            case IR::ThreeAddrInstr::Opcode::GT:
                res = this->compGT(tryArg1, tryArg2);
                break;

            case IR::ThreeAddrInstr::Opcode::GE:
                res = this->compGE(tryArg1, tryArg2);
                break;

            case IR::ThreeAddrInstr::Opcode::LT:
                res = this->compLT(tryArg1, tryArg2);
                break;

            case IR::ThreeAddrInstr::Opcode::LE:
                res = this->compLE(tryArg1, tryArg2);
                break;

            case IR::ThreeAddrInstr::Opcode::BAD:
            default:
                throw std::domain_error("bad");
        }

        n->SetCompVal(res);
    }

    virtual void visitNodeImm(NodeImm*) override{};

    virtual void visitNodeBranch(NodeBranch*) override{};

    virtual void visitNodeRet(NodeRet*) override{};

    virtual void visitInitRegVal(NodeInitRegVal*) override{};

private:
    IR::ImmArg compNEG(IR::ImmArg& arg)
    {
        switch (arg.GetType())
        {
            case IR::DataType::I64:
                return IR::ImmArg(-arg.I64());

            case IR::DataType::I1:
                return IR::ImmArg(!arg.I1());

            case IR::DataType::F64:
                return IR::ImmArg(-arg.F64());

            default:
                return {};
        }
    }

#define binOpArithmTmpl(op)                                                                        \
    if (!arg1.HasType() || !arg1.HasType())                                                        \
        return {};                                                                                 \
                                                                                                   \
    switch (arg1.GetType())                                                                        \
    {                                                                                              \
        case IR::DataType::I64:                                                                    \
            return IR::ImmArg(arg1.I64() op arg2.I64());                                           \
                                                                                                   \
        case IR::DataType::F64:                                                                    \
            return IR::ImmArg(arg1.F64() op arg2.F64());                                           \
                                                                                                   \
        default:                                                                                   \
            return {};                                                                             \
    }

    IR::ImmArg compADD(IR::ImmArg& arg1, IR::ImmArg& arg2){binOpArithmTmpl(+)}

    IR::ImmArg compSUB(IR::ImmArg& arg1, IR::ImmArg& arg2){binOpArithmTmpl(-)}

    IR::ImmArg compMULT(IR::ImmArg& arg1, IR::ImmArg& arg2){binOpArithmTmpl(*)}

    IR::ImmArg compDIV(IR::ImmArg& arg1, IR::ImmArg& arg2)
    {

        if (arg2.GetType() == IR::DataType::I64 && arg2.I64() == 0)
            return {};

        if (arg2.GetType() == IR::DataType::F64 && arg2.F64() == 0.0)
            return {};

        binOpArithmTmpl(/)
    }

#undef binOpArithmTmpl

#define binOpLogicTmpl(op)                                                                         \
    if (!arg1.HasType() || !arg1.HasType())                                                        \
        return {};                                                                                 \
                                                                                                   \
    switch (arg1.GetType())                                                                        \
    {                                                                                              \
        case IR::DataType::I64:                                                                    \
            return IR::ImmArg(arg1.I64() op arg2.I64());                                           \
                                                                                                   \
        case IR::DataType::F64:                                                                    \
            return IR::ImmArg(arg1.F64() op arg2.F64());                                           \
                                                                                                   \
        case IR::DataType::I1:                                                                     \
            return IR::ImmArg(arg1.I1() op arg2.I1());                                             \
                                                                                                   \
        default:                                                                                   \
            return {};                                                                             \
    }

    IR::ImmArg compEQ(IR::ImmArg& arg1, IR::ImmArg& arg2){binOpLogicTmpl(==)}

    IR::ImmArg compNEQ(IR::ImmArg& arg1, IR::ImmArg& arg2){binOpLogicTmpl(!=)}

    IR::ImmArg compGT(IR::ImmArg& arg1, IR::ImmArg& arg2){binOpLogicTmpl(>)}

    IR::ImmArg compGE(IR::ImmArg& arg1, IR::ImmArg& arg2){binOpLogicTmpl(>=)}

    IR::ImmArg compLT(IR::ImmArg& arg1, IR::ImmArg& arg2){binOpLogicTmpl(<)}

    IR::ImmArg compLE(IR::ImmArg& arg1, IR::ImmArg& arg2)
    {
        binOpLogicTmpl(<=)
    }

#undef binOpLogicTmpl
};

class LivenessFixupDAGVisitor : public DAGVisitor
{
public:
    struct ConcreteThreeAddrInstr
    {
        ConcreteThreeAddrInstr(const NodeThreeAddr* _node, const NodeThreeAddr::BoundInstr& _resReg)
            : node(_node), bndInstr(_resReg)
        {
        }

        ArgDefLoc LocOfArg1() const
        {
            return ArgDefLoc(this->node->GetFirstArg(), bndInstr.arg1Idx);
        }

        ArgDefLoc LocOfArg2() const
        {
            return ArgDefLoc(this->node->GetSecondArg(), bndInstr.arg2Idx);
        }

        const IR::ThreeAddrInstr* GetSrcInstr() { return bndInstr.instr; }

        ArgLivenessData GetArgLivenessByNum(size_t num)
        {
            return bndInstr.liveness.GetArgLivenessByNum(num);
        }

    private:
        const NodeThreeAddr*             node;
        const NodeThreeAddr::BoundInstr& bndInstr;
    };

    virtual void visitThreeAddr(NodeThreeAddr* n) override
    {

        for (auto&& bndInstr : n->GetBoundInstrsArrRef())
        {
            ConcreteThreeAddrInstr currThreeAddr(n, bndInstr);

            if (n->GetComputedVal().HasType() || !bndInstr.liveness.GetResLiveness().IsAlive())
            {
                this->tryKillInstrArgs(currThreeAddr);
                continue;
            }

            if (n->GetFirstArg().get()->GetComputedVal().HasType())
            {
                this->tryKillInstrArg(currThreeAddr.LocOfArg1(),
                                      bndInstr.liveness.GetArgLivenessByNum(1), bndInstr.instr);
            }

            if (n->GetSecondArg().get()->GetComputedVal().HasType())
            {
                this->tryKillInstrArg(currThreeAddr.LocOfArg2(),
                                      bndInstr.liveness.GetArgLivenessByNum(2), bndInstr.instr);
            }
        }
    }

    virtual void visitNodeImm(NodeImm*) override {}

    virtual void visitNodeBranch(NodeBranch* n) override
    {
        if (n->GetCondArg().argDefNode.get()->GetComputedVal().HasType())
        {

            this->tryKillInstrArg(n->GetCondArg(), n->GetCondLiveness(), n->GetSrcInst());
        }
    }

    virtual void visitNodeRet(NodeRet* n) override
    {

        if (n->GetRetArg().argDefNode.get()->GetComputedVal().HasType())
        {

            this->tryKillInstrArg(n->GetRetArg(), n->GetRetLiveness(), n->GetSrcInst());
        }
    }

    virtual void visitInitRegVal(NodeInitRegVal*) override {}

private:
    void tryKillInstrArgs(ConcreteThreeAddrInstr instr)
    {
        this->tryKillInstrArg(instr.LocOfArg1(), instr.GetArgLivenessByNum(1), instr.GetSrcInstr());

        this->tryKillInstrArg(instr.LocOfArg2(), instr.GetArgLivenessByNum(2), instr.GetSrcInstr());
    }

    void tryKillInstrArg(ArgDefLoc arg, ArgLivenessData newLive, const IR::Instruction* from)
    {
        NodeThreeAddr* argDAGNode =
            dynamic_cast<NodeThreeAddr*>(arg.argDefNode.get()); // TODO: call instr
        if (argDAGNode == nullptr)
            return;

        NodeThreeAddr::BoundInstr& argInstr = argDAGNode->GetBoundInstrRef(arg.regIdxInNode);

        if (argInstr.liveness.GetResLiveness().GetNextInstr() ==
            from) // kill it only if we're from next use of this instr
        {
            argInstr.liveness.SetLivenessToRes(newLive);

            if (!newLive.IsAlive())
                this->tryKillInstrArgs(ConcreteThreeAddrInstr(argDAGNode, argInstr));
        }
    }
};

class ImmFixupsDAGVisitor : public DAGVisitor
{
public:
    ImmFixupsDAGVisitor(NodeList& n) : nodes(n) {}

    virtual void visitThreeAddr(NodeThreeAddr* n) override
    {
        if (n->GetComputedVal().HasType())
        {
            NodePtr newImmNode = std::make_shared<NodeImm>(n->GetComputedVal());
            n->ReduceToStore(this->nodes.AddNode(newImmNode));

            return;
        }

        NodePtr newImmNode1 = std::make_shared<NodeImm>(n->GetFirstArg().get()->GetComputedVal());
        NodePtr newImmNode2 = std::make_shared<NodeImm>(n->GetSecondArg().get()->GetComputedVal());

        newImmNode1 = this->nodes.AddNode(newImmNode1);
        newImmNode2 = this->nodes.AddNode(newImmNode2);

        n->ChangeArgsToComputed(newImmNode1, newImmNode2);
    }

    virtual void visitNodeImm(NodeImm*) override {}

    virtual void visitNodeBranch(NodeBranch* n) override
    {

        NodePtr newImmNode =
            std::make_shared<NodeImm>(n->GetCondArg().argDefNode.get()->GetComputedVal());

        newImmNode = this->nodes.AddNode(newImmNode);

        n->ChangeCondToComputed(newImmNode);
    }

    virtual void visitNodeRet(NodeRet* n) override
    {
        NodePtr newImmNode =
            std::make_shared<NodeImm>(n->GetRetArg().argDefNode.get()->GetComputedVal());

        newImmNode = this->nodes.AddNode(newImmNode);

        n->ChangeRetArgToComputed(newImmNode);
    }

    virtual void visitInitRegVal(NodeInitRegVal*) override {}

private:
    NodeList& nodes;
};

void Optimizer::Optimize()
{

    FoldOptDAGVisitor foldVis;

    for (auto&& n : this->nodes)
    {
        n.get()->visit(&foldVis);
    }

    LivenessFixupDAGVisitor liveVis;

    for (auto&& n : this->nodes)
    {
        n.get()->visit(&liveVis);
    }

    ImmFixupsDAGVisitor immVis(this->nodes);

    for (auto&& n : this->nodes)
    {
        n.get()->visit(&immVis);
    }
}

} // namespace DAG

} // namespace Opt
