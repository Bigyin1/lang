#pragma once

#include <list>

#include "ir_comp/instructions.hpp"
#include "optimizer/liveness.hpp"

namespace Opt
{
namespace DAG
{

class NodeThreeAddr;

class NodeImm;

class NodeBranch;

class NodeRet;

class NodeInitRegVal;

class NodeNoOp;

class DAGVisitor
{
public:
    virtual void visitThreeAddr(NodeThreeAddr* n) = 0;

    virtual void visitNodeImm(NodeImm* n) = 0;

    virtual void visitNodeBranch(NodeBranch* n) = 0;

    virtual void visitNodeRet(NodeRet* n) = 0;

    virtual void visitInitRegVal(NodeInitRegVal* n) = 0;

    virtual ~DAGVisitor() = default;
};

typedef size_t      BoundInstrIdx;
const BoundInstrIdx NoInstr = (BoundInstrIdx)-1;

class Node
{
public:
    virtual bool operator==(const Node& n) = 0;

    virtual BoundInstrIdx getBoundInstrIdx(IR::RegPtr r) = 0;

    virtual void visit(DAGVisitor* v) = 0;

    virtual IR::ImmArg GetComputedVal() = 0;

    virtual ~Node() = default;
};

typedef std::shared_ptr<Node> NodePtr;

class NodeNoOp : public Node
{
public:
    virtual BoundInstrIdx getBoundInstrIdx(IR::RegPtr) override { return NoInstr; }
    virtual bool          operator==(const Node&) override { return false; }

    virtual void visit(DAGVisitor*) override {}

    virtual IR::ImmArg GetComputedVal() override { return {}; }
};

struct ArgDefLoc
{
    ArgDefLoc() : argDefNode(nullptr), regIdxInNode(NoInstr) {}

    ArgDefLoc(NodePtr _argDefNode, BoundInstrIdx _regInNode)
        : argDefNode(_argDefNode), regIdxInNode(_regInNode)
    {
    }

    NodePtr       argDefNode;
    BoundInstrIdx regIdxInNode;
};

class NodeThreeAddr : public Node
{
public:
    NodeThreeAddr(IR::ThreeAddrInstr::Opcode _op, NodePtr _arg1, NodePtr _arg2)
        : op(_op), arg1(_arg1), arg2(_arg2)
    {
    }

    virtual BoundInstrIdx getBoundInstrIdx(IR::RegPtr r) override;

    virtual void visit(DAGVisitor* v) override { v->visitThreeAddr(this); }

    struct BoundInstr
    {
        BoundInstr(const IR::ThreeAddrInstr* i, InstrLivenessData rl, BoundInstrIdx a1i,
                   BoundInstrIdx a2i)
            : instr(i), resReg(i->regRes.get()->GetReg()), liveness(rl), arg1Idx(a1i), arg2Idx(a2i)
        {
        }

        const IR::ThreeAddrInstr* instr;

        IR::RegPtr        resReg;
        InstrLivenessData liveness;
        BoundInstrIdx     arg1Idx;
        BoundInstrIdx     arg2Idx;
    };

    virtual bool operator==(const Node& n) override;

    virtual IR::ImmArg GetComputedVal() override { return this->comp; }

    void AddBoundInstr(BoundInstr r) { this->boundInstrs.push_back(r); }

    BoundInstr& GetBoundInstrRef(BoundInstrIdx idx) { return this->boundInstrs[idx]; }

    IR::ThreeAddrInstr::Opcode GetOp() const { return this->op; }
    void                       SetOp(IR::ThreeAddrInstr::Opcode op) { this->op = op; }

    void SetCompVal(IR::ImmArg comp) { this->comp = comp; }

    NodePtr GetFirstArg() const { return this->arg1; }
    NodePtr GetSecondArg() const { return this->arg2; }

    std::vector<BoundInstr>& GetBoundInstrsArrRef() { return this->boundInstrs; }

    void ReduceToStore(NodePtr arg)
    {
        this->op = IR::ThreeAddrInstr::Opcode::STORE;

        this->arg1 = arg;

        this->arg2 = std::make_shared<NodeNoOp>();
    }

    void ChangeArgsToComputed(NodePtr newImmNode1, NodePtr newImmNode2)
    {
        if (newImmNode1.get()->GetComputedVal().HasType())
            this->arg1 = newImmNode1;

        if (newImmNode2.get()->GetComputedVal().HasType())
            this->arg2 = newImmNode2;
    }

private:
    IR::ThreeAddrInstr::Opcode op;

    std::vector<BoundInstr> boundInstrs = {};

    NodePtr arg1;
    NodePtr arg2;

    IR::ImmArg comp = {};
};

class NodeImm : public Node
{
public:
    NodeImm(const IR::ImmArg arg) : val(arg) {}

    virtual BoundInstrIdx getBoundInstrIdx(IR::RegPtr) override;

    virtual bool operator==(const Node& n) override;

    virtual void visit(DAGVisitor* v) override { v->visitNodeImm(this); }

    virtual IR::ImmArg GetComputedVal() override { return val; }

    IR::ImmArg GetVal() const { return val; }

private:
    IR::ImmArg val;
};

class NodeBranch : public Node
{
public:
    NodeBranch(ArgDefLoc c, InstrLivenessData l, const IR::Instruction* si)
        : condArg(c), liveness(l), srcInstr(si)
    {
    }

    virtual bool operator==(const Node&) override { return false; }

    virtual BoundInstrIdx getBoundInstrIdx(IR::RegPtr) override { return NoInstr; }

    virtual void visit(DAGVisitor* v) override { v->visitNodeBranch(this); }

    virtual IR::ImmArg GetComputedVal() override { return {}; }

    ArgDefLoc GetCondArg() const { return condArg; }

    ArgLivenessData GetCondLiveness() const { return liveness.GetBrCondLiveness(); }

    const IR::Instruction* GetSrcInst() { return srcInstr; }

    void ChangeCondToComputed(NodePtr newImmNode)
    {
        if (newImmNode.get()->GetComputedVal().HasType())
            this->condArg.argDefNode = newImmNode;
    }

private:
    ArgDefLoc condArg;

    InstrLivenessData liveness;

    const IR::Instruction* srcInstr;
};

class NodeRet : public Node
{
public:
    NodeRet(ArgDefLoc r, InstrLivenessData l, const IR::Instruction* si)
        : retArg(r), liveness(l), srcInstr(si)
    {
    }

    virtual BoundInstrIdx getBoundInstrIdx(IR::RegPtr) override { return NoInstr; }

    virtual bool operator==(const Node&) override { return false; }

    virtual void visit(DAGVisitor* v) override { v->visitNodeRet(this); }

    virtual IR::ImmArg GetComputedVal() override { return {}; }

    ArgDefLoc GetRetArg() const { return retArg; }

    ArgLivenessData GetRetLiveness() const { return liveness.GetRetValLiveness(); }

    const IR::Instruction* GetSrcInst() { return srcInstr; }

    void ChangeRetArgToComputed(NodePtr newImmNode)
    {
        if (newImmNode.get()->GetComputedVal().HasType())
            this->retArg.argDefNode = newImmNode;
    }

private:
    ArgDefLoc retArg;

    InstrLivenessData liveness;

    const IR::Instruction* srcInstr;
};

class NodeInitRegVal : public Node
{
public:
    NodeInitRegVal(IR::RegPtr r) : reg(r) {}

    virtual BoundInstrIdx getBoundInstrIdx(IR::RegPtr r) override
    {
        if (reg == r)
            return 0;

        return NoInstr;
    }
    virtual bool operator==(const Node&) override { return false; }

    virtual void visit(DAGVisitor* v) override { v->visitInitRegVal(this); }

    virtual IR::ImmArg GetComputedVal() override { return {}; }

    IR::RegPtr GetReg() const { return reg; }

private:
    IR::RegPtr reg;

    // TODO: interblock analysys
};

class NodeList
{
public:
    ArgDefLoc FindLastRegDefNode(IR::RegPtr reg);
    NodePtr   AddNode(NodePtr n);

    using iterator = typename std::list<NodePtr>::iterator;
    iterator begin() { return nodes.begin(); }
    iterator end() { return nodes.end(); }

private:
    std::list<NodePtr> nodes = {};
};

class DAGBuilderIRArgVisitor;
class DAGBuilderIRVisitor;

class Builder
{
    friend class DAGBuilderIRVisitor;
    friend class DAGBuilderIRArgVisitor;

public:
    Builder(NodeList& n, const IR::BasicBlock* b, LivenessData& ld)
        : nodes(n), block(b), livenessData(ld)
    {
    }

    void BuildDAG();

private:
    NodeList& nodes;

    const IR::BasicBlock* block;

    LivenessData& livenessData;
};

class Optimizer
{
public:
    Optimizer(NodeList& n) : nodes(n) {}

    void Optimize();

private:
    NodeList& nodes;
};

} // namespace DAG

} // namespace Opt
