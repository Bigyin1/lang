/** @file */
#pragma once

#include <map>
#include <stack>

#include "ir_comp/ir.hpp"
#include "parser/parser.hpp"
#include "semantic/scope.hpp"

class WalkerCtx
{

public:
    WalkerCtx(ScopeNode* sc) : currScope(sc) {}

    ScopeNode* currScope;
};

class IRCompiler
{

public:
    IRCompiler(Node nr, ScopeNode* rs) : astRoot(nr), rootScope(rs) {}
    void RunAstToIRCompiler();

    void Print(std::ostream& out) const
    {
        for (auto&& i : funcs)
        {
            i.second.get()->print(out);
            out << std::endl;
        }
    }

    std::map<IR::FuncName, std::unique_ptr<IR::Function>> funcs = {};

private:
    typedef std::pair<IR::BasicBlockName, IR::BasicBlockName> ClauseBlocks;
    typedef std::unique_ptr<IR::InstrArg>                     ArgPtr;

    void walkForStmtNode(const ForStmtNode* forstn);
    void walkIfStmtNode(const IfStmtNode* ifstn);

    ArgPtr walkValNode(const ValNode* vn);

    void walkLogic(const BinOpNode* bopn, ClauseBlocks cb);

    void   walkAssignNode(const BinOpNode* bopn);
    ArgPtr walkArithmOrRelNode(const BinOpNode* bopn, IR::RegPtr resReg);

    ArgPtr walkFuncCallNode(const FuncCallNode* fcn, IR::RegPtr resReg);
    ArgPtr walkUnOpNode(const UnOpNode* uopn, IR::RegPtr resReg);

    void walkClauseArgNode(const Node n, ClauseBlocks cb);

    ArgPtr walkSimpleStmtNewReg(const Node n);
    void   walkSimpleStmtToReg(const Node n, IR::RegPtr resReg);

    ArgPtr walkSimpleStmt(const Node n, IR::RegPtr resReg);

    void walkVarDecl(const VarDeclNode* vdn);
    void buildFuncDef(FuncDeclNode* fdn);
    void walkCompStmtCtx(const ListNode* compStmt);
    void walkCompStmt(const ListNode* body);
    void walkFuncBody(const ListNode* body);
    void walkRoot();

    ScopeNode* GetCurrScope() { return this->ctxStack.top().currScope; }

    static IR::ThreeAddrInstr::Opcode langBinOpToIROp(TokenName op);

    IR::RegPtr newRegister(IR::DataType t)
    {
        std::string rName;
        if (t != IR::DataType::Void)
            rName = std::to_string(this->GetNextID());

        return std::make_shared<IR::Register>(rName, t);
    }

    void addNewBlockWithName(IR::BasicBlockName n)
    {
        auto bl          = std::make_unique<IR::BasicBlock>(n);
        this->currBBlock = bl.get();
        this->currFunc->blocks.push_back(std::move(bl));
    }

    void addInstrToCurrBlock(std::unique_ptr<IR::Instruction> instr)
    {
        this->currBBlock->instrs.push_back(std::move(instr));
    }

    void nextCtx()
    {
        this->ctxStack.push(WalkerCtx(ScopeGetCurrChild(this->ctxStack.top().currScope)));
    }

    void prevCtx() { this->ctxStack.pop(); }

    void AddVar(const Symbol* varSymb, IR::RegPtr regP) { this->var2reg[varSymb] = regP; }

    IR::RegPtr GetRegForSymb(const Symbol* varSymb) { return this->var2reg[varSymb]; }

    Node       astRoot;
    ScopeNode* rootScope;

    std::stack<WalkerCtx> ctxStack;

    std::stack<ClauseBlocks> currForStmtBlocks;

    unsigned GetNextID() { return nextId++; }
    unsigned nextId = 0;

    IR::Function*   currFunc   = nullptr;
    IR::BasicBlock* currBBlock = nullptr;

    std::map<const Symbol*, IR::RegPtr> var2reg;
};
