#include "ir_comp/instructions.hpp"
#include "optimizer/liveness.hpp"

namespace Opt
{
namespace DAG
{

class Node
{
public:
    virtual bool operator==(const Node& n) = 0;

    virtual std::pair<bool, size_t> boundRegLoc(IR::RegPtr r) = 0;

    virtual ~Node() = default;
};

typedef std::shared_ptr<Node> NodePtr;

typedef std::pair<NodePtr, size_t> ArgDefLoc;

class NodeThreeAddr : public Node
{
public:
    NodeThreeAddr(IR::ThreeAddrInstr::Opcode op, NodePtr arg1, NodePtr arg2)
        : op(op), arg1(arg1), arg2(arg2)
    {
    }

    virtual std::pair<bool, size_t> boundRegLoc(IR::RegPtr r) override;

    struct BoundReg
    {
        BoundReg(IR::RegPtr r, InstrLivenessData rl, size_t a1i, size_t a2i)
            : reg(r), liveness(rl), arg1Idx(a1i), arg2Idx(a2i)
        {
        }

        IR::RegPtr        reg;
        InstrLivenessData liveness;
        size_t            arg1Idx;
        size_t            arg2Idx;
    };

    void AddBoundReg(BoundReg r) { this->boundRegs.push_back(r); }

    virtual bool operator==(const Node& n) override;

    IR::ThreeAddrInstr::Opcode op;

    std::vector<BoundReg> boundRegs;

    NodePtr arg1;
    NodePtr arg2;
};

class NodeImm : public Node
{
public:
    NodeImm(const IR::ImmArg* arg) : val(*arg) {}

    virtual std::pair<bool, size_t> boundRegLoc(IR::RegPtr) override;

    virtual bool operator==(const Node& n) override;

    IR::ImmArg val;
};

class NodeBranch : public Node
{
public:
    NodeBranch(ArgDefLoc cond, InstrLivenessData liveness, const IR::Instruction* srcInstr)
        : condArg(cond), liveness(liveness), srcInstr(srcInstr)
    {
    }

    virtual bool operator==(const Node&) override { return false; }

    virtual std::pair<bool, size_t> boundRegLoc(IR::RegPtr) override { return {false, 0}; }

    ArgDefLoc condArg;

    InstrLivenessData liveness;

    const IR::Instruction* srcInstr;
};

class NodeRet : public Node
{
public:
    NodeRet(ArgDefLoc ret, InstrLivenessData liveness) : retArg(ret), liveness(liveness) {}

    virtual std::pair<bool, size_t> boundRegLoc(IR::RegPtr) override { return {false, 0}; }

    virtual bool operator==(const Node&) override { return false; }

    ArgDefLoc retArg;

    InstrLivenessData liveness;
};

class NodeInitRegVal : public Node
{
public:
    NodeInitRegVal(IR::RegPtr r) : reg(r) {}

    virtual std::pair<bool, size_t> boundRegLoc(IR::RegPtr r) override
    {

        if (reg == r)
            return {true, 0};

        return {false, 0};
    }
    virtual bool operator==(const Node&) override { return false; }

    IR::RegPtr reg;

    // TODO: interblock analysys
};

class DAGBuilderIRArgVisitor;
class DAGBuilderIRVisitor;

class Builder
{
    friend class DAGBuilderIRVisitor;
    friend class DAGBuilderIRArgVisitor;

public:
    Builder(IR::BasicBlock* b, LivenessData& ld) : block(b), livenessData(ld) {}

    void BuildDAG();

private:
    ArgDefLoc findLastRegDefNode(IR::RegPtr reg);

    NodePtr getOrAddNode(NodePtr n);

    std::vector<NodePtr> nodes;

    IR::BasicBlock* block;

    LivenessData& livenessData;
};

} // namespace DAG

} // namespace Opt
