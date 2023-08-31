#include "optimizer/dag.hpp"

namespace Opt
{

namespace DAG
{

ArgDefLoc NodeList::FindLastRegDefNode(IR::RegPtr reg)
{

    for (auto node = this->nodes.rbegin(); node != this->nodes.rend(); ++node)
    {
        BoundInstrIdx regIdx = (*node).get()->getBoundInstrIdx(reg);
        if (regIdx != NoInstr)
            return ArgDefLoc(*node, regIdx);
    }

    return ArgDefLoc(nullptr, NoInstr);
}

NodePtr NodeList::AddNode(NodePtr n)
{

    for (auto&& node : this->nodes)
    {
        if (*n.get() == *node.get())
            return node;
    }

    this->nodes.push_back(n);

    return n;
}

class DAGBuilderIRArgVisitor : public IR::ArgVisitor
{

public:
    DAGBuilderIRArgVisitor(Builder& b) : builder(b) {}

    virtual void visitRegArg(const IR::RegArg* arg) override
    {

        this->argDefLoc = builder.nodes.FindLastRegDefNode(arg->GetReg());
        if (this->argDefLoc.argDefNode == nullptr)
        {
            auto node       = std::make_shared<NodeInitRegVal>(arg->GetReg());
            this->argDefLoc = ArgDefLoc(node, 0);

            builder.nodes.AddNode(node);
        }
    }

    virtual void visitImmArg(const IR::ImmArg* arg) override
    {

        NodePtr newNode = std::make_shared<NodeImm>(*arg);

        newNode = this->builder.nodes.AddNode(newNode);

        this->argDefLoc = {newNode, 0};
    }

    ArgDefLoc GetRes() { return this->argDefLoc; }

private:
    Builder&  builder;
    ArgDefLoc argDefLoc;
};

class DAGBuilderIRVisitor : public IR::InstrVisitor
{

public:
    DAGBuilderIRVisitor(Builder& b) : builder(b) {}

    virtual void visitThreeAddr(const IR::ThreeAddrInstr* instr) override
    {
        InstrLivenessData instrLiv = this->builder.livenessData[instr];

        ArgDefLoc arg1Loc;
        ArgDefLoc arg2Loc;

        DAGBuilderIRArgVisitor argVis(builder);

        instr->arg1.get()->visit(&argVis);
        arg1Loc = argVis.GetRes();

        if (instr->arg2 != nullptr)
        {
            instr->arg2.get()->visit(&argVis);
            arg2Loc = argVis.GetRes();
        }
        else
        {
            arg2Loc.argDefNode = std::make_shared<NodeNoOp>();
        }

        NodePtr newNode =
            std::make_shared<NodeThreeAddr>(instr->op, arg1Loc.argDefNode, arg2Loc.argDefNode);

        newNode = this->builder.nodes.AddNode(newNode);

        NodeThreeAddr* newTA = dynamic_cast<NodeThreeAddr*>(newNode.get());
        newTA->AddBoundInstr(
            NodeThreeAddr::BoundInstr(instr, instrLiv, arg1Loc.regIdxInNode, arg2Loc.regIdxInNode));
    }

    virtual void visitBranchInstr(const IR::BranchInstr* instr) override
    {

        ArgDefLoc condArg = {};

        DAGBuilderIRArgVisitor argVis(builder);

        if (instr->cond != nullptr)
        {
            instr->cond.get()->visit(&argVis);
            condArg = argVis.GetRes();
        }
        else
        {
            condArg.argDefNode = std::make_shared<NodeNoOp>();
        }

        InstrLivenessData instrLiv = this->builder.livenessData[instr];
        NodePtr           newNode  = std::make_shared<NodeBranch>(condArg, instrLiv, instr);

        this->builder.nodes.AddNode(newNode);
    }

    virtual void visitCallInstr(const IR::CallInstr*) override {}

    virtual void visitRetInstr(const IR::RetInstr* instr) override
    {
        ArgDefLoc retArg = {};

        DAGBuilderIRArgVisitor argVis(builder);

        if (instr->retVal != nullptr)
        {
            instr->retVal.get()->visit(&argVis);
            retArg = argVis.GetRes();
        }
        else
        {
            retArg.argDefNode = std::make_shared<NodeNoOp>();
        }

        InstrLivenessData instrLiv = this->builder.livenessData[instr];
        NodePtr           newNode  = std::make_shared<NodeRet>(retArg, instrLiv, instr);

        this->builder.nodes.AddNode(newNode);
    }

private:
    Builder& builder;
};

BoundInstrIdx NodeThreeAddr::getBoundInstrIdx(IR::RegPtr r)
{

    for (ssize_t i = (ssize_t)this->boundInstrs.size() - 1; i >= 0; i--)
    {
        if (this->boundInstrs[(size_t)i].resReg == r)
            return (BoundInstrIdx)i;
    }

    return NoInstr;
}

bool NodeThreeAddr::operator==(const Node& n)
{

    auto same = dynamic_cast<const NodeThreeAddr*>(&n);
    if (same == nullptr)
        return false;

    if (same->op != this->op)
        return false;

    if (this->op == IR::ThreeAddrInstr::Opcode::ADD || this->op == IR::ThreeAddrInstr::Opcode::MUL)
    {
        return (arg1 == same->arg1 && arg2 == same->arg2) ||
               (arg1 == same->arg2 && arg2 == same->arg1);
    }

    return arg1 == same->arg1 && arg2 == same->arg2;
}

BoundInstrIdx NodeImm::getBoundInstrIdx(IR::RegPtr) { return NoInstr; }

bool NodeImm::operator==(const Node& n)
{

    auto same = dynamic_cast<const NodeImm*>(&n);
    if (same == nullptr)
        return false;

    if (same->val.GetType() != this->val.GetType())
        return false;

    return same->val.I64() == this->val.I64();
}

void Builder::BuildDAG()
{
    DAGBuilderIRVisitor v(*this);

    for (auto&& instr : this->block->instrs)
    {
        instr.get()->visit(&v);
    }
}

} // namespace DAG

} // namespace Opt
