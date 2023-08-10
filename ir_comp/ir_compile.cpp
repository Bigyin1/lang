#include <map>

#include "ir_comp/compiler.hpp"

static IR::DataType langTypeToIRType(const Type* t)
{

    switch (t->bt)
    {
        case BaseTypeInteger:
            return IR::DataType::I64;

        case BaseTypeFloat64:
            return IR::DataType::F64;

        case BaseTypeBoolean:
            return IR::DataType::I1;

        case BaseTypeEmpty:
            return IR::DataType::Void;
    }
}

static IR::DataType opResDataType(TokenName op, IR::DataType argType)
{

    if (op & (TOK_MULT | TOK_DIV | TOK_PLUS | TOK_MINUS))
    {
        return argType;
    }

    return IR::DataType::I1;
}

IR::ThreeAddrInstr::Opcode IRCompiler::langBinOpToIROp(TokenName op)
{
    switch (op)
    {
        case TOK_PLUS:
            return IR::ThreeAddrInstr::Opcode::ADD;
        case TOK_MINUS:
            return IR::ThreeAddrInstr::Opcode::SUB;
        case TOK_MULT:
            return IR::ThreeAddrInstr::Opcode::MUL;
        case TOK_DIV:
            return IR::ThreeAddrInstr::Opcode::DIV;
        case TOK_NE:
            return IR::ThreeAddrInstr::Opcode::NEQ;
        case TOK_GE:
            return IR::ThreeAddrInstr::Opcode::GE;
        case TOK_GT:
            return IR::ThreeAddrInstr::Opcode::GT;
        case TOK_LE:
            return IR::ThreeAddrInstr::Opcode::LE;
        case TOK_LT:
            return IR::ThreeAddrInstr::Opcode::LT;

        case TOK_NOT:
        default:
            return IR::ThreeAddrInstr::Opcode::BAD;
    }
}

std::unique_ptr<IR::InstrArg> IRCompiler::walkValNode(const ValNode* v)
{

    TokenName valType = v->val->n;

    std::unique_ptr<IR::InstrArg> valArgPtr;

    if (valType == TOK_TRUE)
        valArgPtr = std::make_unique<IR::ImmArg>(true);

    if (valType == TOK_FALSE)
        valArgPtr = std::make_unique<IR::ImmArg>(false);

    if (valType == TOK_Integer)
        valArgPtr = std::make_unique<IR::ImmArg>(v->val->IntVal);

    if (valType == TOK_Float)
        valArgPtr = std::make_unique<IR::ImmArg>(v->val->FloatVal);

    if (valType == TOK_ID)
    {
        Symbol* sym = GetSymbolByNameFromScope(this->GetCurrScope(), v->val->StrVal);

        auto reg  = this->GetRegForSymb(sym);
        valArgPtr = std::make_unique<IR::RegArg>(reg);
    }

    return valArgPtr;
}

void IRCompiler::walkLogic(const BinOpNode* bopn, ClauseBlocks cb)
{
    IR::BasicBlockName blockTrue  = cb.first;
    IR::BasicBlockName blockFalse = cb.second;

    auto rightBlockName = std::to_string(this->GetNextID());

    std::pair<IR::BasicBlockName, IR::BasicBlockName> nextL;
    std::pair<IR::BasicBlockName, IR::BasicBlockName> nextR;

    if (bopn->op->n & TOK_LOR)
    {
        nextL.first  = blockTrue;
        nextL.second = rightBlockName;
    }
    else
    {
        nextL.first  = rightBlockName;
        nextL.second = blockFalse;
    }
    nextR.first  = blockTrue;
    nextR.second = blockFalse;

    this->walkClauseArgNode(bopn->left, nextL);

    this->addNewBlockWithName(rightBlockName);

    this->walkClauseArgNode(bopn->right, nextR);
}

void IRCompiler::walkAssignNode(const BinOpNode* bopn)
{
    Symbol* sym = GetSymbolByNameFromScope(this->GetCurrScope(), bopn->left.vn->val->StrVal);
    auto    reg = this->GetRegForSymb(sym);

    this->walkSimpleStmtToReg(bopn->right, reg);
}

std::unique_ptr<IR::InstrArg> IRCompiler::walkArithmOrRelNode(const BinOpNode* bopn,
                                                              IR::RegPtr       resReg)
{
    std::unique_ptr<IR::InstrArg> arg1 = this->walkSimpleStmtNewReg(bopn->left);
    std::unique_ptr<IR::InstrArg> arg2 = this->walkSimpleStmtNewReg(bopn->right);

    auto irOpCode = langBinOpToIROp(bopn->op->n);

    auto res = resReg == nullptr
                   ? this->newRegister(opResDataType(bopn->op->n, arg1.get()->GetType()))
                   : resReg;

    auto instr =
        std::make_unique<IR::ThreeAddrInstr>(irOpCode, std::move(arg1), std::move(arg2), res);

    this->addInstrToCurrBlock(std::move(instr));

    return std::make_unique<IR::RegArg>(res);
}

std::unique_ptr<IR::InstrArg> IRCompiler::walkUnOpNode(const UnOpNode* uopn, IR::RegPtr resReg)
{
    std::unique_ptr<IR::InstrArg> arg = this->walkSimpleStmtNewReg(uopn->child);

    auto irOpCode = IR::ThreeAddrInstr::Opcode::NEG;

    auto res = resReg == nullptr
                   ? this->newRegister(opResDataType(uopn->op->n, arg.get()->GetType()))
                   : resReg;

    auto instr = std::make_unique<IR::ThreeAddrInstr>(irOpCode, std::move(arg), res);

    this->addInstrToCurrBlock(std::move(instr));

    return std::make_unique<IR::RegArg>(res);
}

std::unique_ptr<IR::InstrArg> IRCompiler::walkFuncCallNode(const FuncCallNode* fcn,
                                                           IR::RegPtr          resReg)
{
    IR::FuncName fName = std::string(fcn->fName->StrVal);

    std::vector<std::unique_ptr<IR::InstrArg>> args;

    const ListNode* rawArgs = fcn->args;
    for (size_t i = 0; i < rawArgs->chLen; i++)
    {
        args.push_back(walkSimpleStmtNewReg(rawArgs->children[i]));
    }

    Symbol* funcSymb = GetSymbolByNameFromScope(this->GetCurrScope(), fcn->fName->StrVal);

    auto retType = langTypeToIRType(funcSymb->type->ft.retType);

    IR::RegPtr res = resReg == nullptr ? this->newRegister(retType) : resReg;

    auto instr = std::make_unique<IR::CallInstr>(std::move(fName), std::move(args), res);

    this->addInstrToCurrBlock(std::move(instr));

    return std::make_unique<IR::RegArg>(res);
}

void IRCompiler::walkClauseArgNode(const Node n, ClauseBlocks cb)
{
    NodeType nt = n.hdr.type;

    TokenName tokN = n.bopn->op->n;

    if (nt == NODE_BINOP && (tokN & (TOK_LOR | TOK_LAND))) // short circuit logical
    {
        this->walkLogic(n.bopn, cb);
        return;
    }

    std::unique_ptr<IR::InstrArg> resPtr;

    if (nt == NODE_BINOP && (tokN & (TOK_EQ | TOK_GE | TOK_GT | TOK_LE | TOK_LT | TOK_NE)))
    {
        resPtr = this->walkArithmOrRelNode(n.bopn, nullptr);
    }
    else if (nt == NODE_UNOP)
    {
        resPtr = this->walkUnOpNode(n.uopn, nullptr);
    }
    else if (nt == NODE_FUNCTION_CALL)
    {
        resPtr = this->walkFuncCallNode(n.fcn, nullptr);
    }
    else if (nt == NODE_VAL)
    {
        resPtr = this->walkValNode(n.vn);
    }

    auto brInstr = std::make_unique<IR::BranchInstr>(std::move(resPtr), cb.first, cb.second);
    this->addInstrToCurrBlock(std::move(brInstr));
}

std::unique_ptr<IR::InstrArg> IRCompiler::walkSimpleStmt(const Node n, IR::RegPtr resReg)
{

    NodeType nt = n.hdr.type;

    if (nt == NODE_BINOP)
    {
        if (n.bopn->op->n & (TOK_LOR | TOK_LAND)) // short circuit boolean store
        {
            IR::RegPtr res = resReg == nullptr ? this->newRegister(IR::DataType::I1) : resReg;

            auto firstInstrPtr = std::make_unique<IR::ThreeAddrInstr>(
                IR::ThreeAddrInstr::Opcode::STORE, std::make_unique<IR::ImmArg>(false), res);
            this->addInstrToCurrBlock(std::move(firstInstrPtr));

            IR::BasicBlockName blockTrue  = std::to_string(this->GetNextID());
            IR::BasicBlockName blockFalse = std::to_string(this->GetNextID());

            this->walkLogic(n.bopn, ClauseBlocks(blockTrue, blockFalse));

            this->addNewBlockWithName(blockTrue);

            auto lastInstrPtr = std::make_unique<IR::ThreeAddrInstr>(
                IR::ThreeAddrInstr::Opcode::STORE, std::make_unique<IR::ImmArg>(true), res);
            this->addInstrToCurrBlock(std::move(lastInstrPtr));

            auto brInstr = std::make_unique<IR::BranchInstr>(blockFalse);
            this->addInstrToCurrBlock(std::move(brInstr));

            this->addNewBlockWithName(blockFalse);

            return std::make_unique<IR::RegArg>(res);
        }

        return this->walkArithmOrRelNode(n.bopn, resReg);
    }

    if (nt == NODE_UNOP)
        return this->walkUnOpNode(n.uopn, resReg);

    if (nt == NODE_FUNCTION_CALL)
        return this->walkFuncCallNode(n.fcn, resReg);

    if (nt == NODE_VAL)
    {
        if (resReg == nullptr)
            return this->walkValNode(n.vn); // just returning value, wrapped in InstrArg class

        // store value, only used in assign operation
        auto instr = std::make_unique<IR::ThreeAddrInstr>(IR::ThreeAddrInstr::Opcode::STORE,
                                                          this->walkValNode(n.vn), resReg);

        this->addInstrToCurrBlock(std::move(instr));

        return nullptr;
    }

    return nullptr;
}

std::unique_ptr<IR::InstrArg> IRCompiler::walkSimpleStmtNewReg(const Node n)
{
    return this->walkSimpleStmt(n, nullptr);
}

void IRCompiler::walkSimpleStmtToReg(const Node n, IR::RegPtr resReg)
{
    this->walkSimpleStmt(n, resReg);
}

void IRCompiler::walkVarDecl(const VarDeclNode* vdn)
{
    Symbol* varSymb = GetSymbolByNameFromScope(this->GetCurrScope(), vdn->varName->StrVal);

    auto varReg = std::make_shared<IR::Register>(std::to_string(this->GetNextID()),
                                                 langTypeToIRType(varSymb->type));

    this->AddVar(varSymb, varReg);

    BinOpNode assign = {0};
    ValNode   varVal = {0};

    varVal.val = vdn->varName;

    Token at    = {.n = TOK_ASSIGN};
    assign.op   = &at;
    assign.left = Node{.vn = &varVal};

    Token   initValTok  = {};
    ValNode initVal     = {};
    Node    initValNode = {};
    if (vdn->initVal.hdr.type != NODE_EMPTY)
        assign.right = vdn->initVal;
    else
    {
        initValNode.hdr.type = NODE_VAL;
        initValNode.vn       = &initVal;
        initVal.val          = &initValTok;

        if (varSymb->type->bt == BaseTypeBoolean)
        {
            initValTok.n = TOK_FALSE;
        }
        else if (varSymb->type->bt == BaseTypeInteger)
        {
            initValTok.n      = TOK_Integer;
            initValTok.IntVal = 0;
        }
        else if (varSymb->type->bt == BaseTypeFloat64)
        {
            initValTok.n        = TOK_Float;
            initValTok.FloatVal = 0.0;
        }

        assign.right = initValNode;
    }

    walkAssignNode(&assign);
}

void IRCompiler::walkIfStmtNode(const IfStmtNode* ifstn)
{
    IR::BasicBlockName blockNameBody = std::to_string(this->GetNextID());
    IR::BasicBlockName blockNameElse = std::to_string(this->GetNextID());

    this->walkClauseArgNode(ifstn->cond, ClauseBlocks(blockNameBody, blockNameElse));

    this->addNewBlockWithName(blockNameBody);

    this->walkCompStmtCtx(ifstn->body);

    auto brInstr = std::make_unique<IR::BranchInstr>(blockNameElse);
    this->addInstrToCurrBlock(std::move(brInstr));

    this->addNewBlockWithName(blockNameElse);

    if (ifstn->elseBody.hdr.type == NODE_IF_STMT)
    {
        this->walkIfStmtNode(ifstn->elseBody.ifstn);
    }
    else if (ifstn->elseBody.hdr.type == NODE_COMPOUND_STMT)
    {
        this->walkCompStmtCtx(ifstn->elseBody.ln);
    }
}

void IRCompiler::walkForStmtNode(const ForStmtNode* forstn)
{

    IR::BasicBlockName blockClause;

    if (!this->currBBlock->instrs.empty())
    {
        blockClause = std::to_string(this->GetNextID());

        auto brInstr = std::make_unique<IR::BranchInstr>(blockClause);
        this->addInstrToCurrBlock(std::move(brInstr));

        this->addNewBlockWithName(blockClause);
    }
    else
    {
        blockClause = this->currBBlock->name;
    }

    IR::BasicBlockName blockNameBody  = std::to_string(this->GetNextID());
    IR::BasicBlockName blockNameAfter = std::to_string(this->GetNextID());

    this->walkClauseArgNode(forstn->cond, ClauseBlocks(blockNameBody, blockNameAfter));

    this->addNewBlockWithName(blockNameBody);

    this->currForStmtBlocks.push(ClauseBlocks(blockClause, blockNameAfter));
    this->walkCompStmtCtx(forstn->body);
    this->currForStmtBlocks.pop();

    auto brInstr = std::make_unique<IR::BranchInstr>(blockClause);
    this->addInstrToCurrBlock(std::move(brInstr));

    this->addNewBlockWithName(blockNameAfter);
}

void IRCompiler::walkCompStmtCtx(const ListNode* compStmt)
{
    this->nextCtx();
    this->walkCompStmt(compStmt);
    this->prevCtx();
}

void IRCompiler::walkCompStmt(const ListNode* compStmt)
{

    for (size_t i = 0; i < compStmt->chLen; i++)
    {
        Node     next = compStmt->children[i];
        NodeType nt   = next.hdr.type;

        if (nt == NODE_VAR_DECL)
        {
            walkVarDecl(next.vdn);
        }
        else if (nt == NODE_BINOP)
        {
            TokenName t = next.bopn->op->n;

            if (t & TOK_ASSIGN)
                walkAssignNode(next.bopn);
        }
        else if (nt == NODE_IF_STMT)
        {
            walkIfStmtNode(next.ifstn);
        }
        else if (nt == NODE_FOR_STMT)
        {
            walkForStmtNode(next.forstn);
        }
        else if (nt == NODE_FUNCTION_CALL)
        {
            walkFuncCallNode(next.fcn, nullptr);
        }
        else if (nt == NODE_COMPOUND_STMT)
        {
            walkCompStmtCtx(next.ln);
        }
        else if (nt == NODE_UNOP)
        {
            TokenName t = next.uopn->op->n;

            if (t & TOK_BREAK)
            {
                ClauseBlocks cb = this->currForStmtBlocks.top();

                auto brInstr = std::make_unique<IR::BranchInstr>(cb.second);
                this->addInstrToCurrBlock(std::move(brInstr));

                this->addNewBlockWithName(std::to_string(this->GetNextID()));
            }
            else if (t & TOK_CONTINUE)
            {
                ClauseBlocks cb = this->currForStmtBlocks.top();

                auto brInstr = std::make_unique<IR::BranchInstr>(cb.first);
                this->addInstrToCurrBlock(std::move(brInstr));

                this->addNewBlockWithName(std::to_string(this->GetNextID()));
            }
            else if (t & TOK_RETURN)
            {
                auto retInst =
                    std::make_unique<IR::RetInstr>(walkSimpleStmtNewReg(next.uopn->child));

                this->addInstrToCurrBlock(std::move(retInst));

                this->addNewBlockWithName(std::to_string(this->GetNextID()));
            }
        }
    }
}

void IRCompiler::walkFuncBody(const ListNode* body)
{

    auto block       = std::make_unique<IR::BasicBlock>(std::to_string(this->GetNextID()));
    this->currBBlock = block.get();

    this->currFunc->blocks.push_back(std::move(block));

    this->walkCompStmt(body);
}

void IRCompiler::buildFuncDef(FuncDeclNode* fdn)
{
    IR::FuncDef& funcDef = this->currFunc->def;

    Symbol*     funcSymb = GetSymbolByNameFromScope(this->GetCurrScope(), fdn->fName->StrVal);
    const Type* funcType = funcSymb->type;

    funcDef.name    = std::string(funcSymb->tok->StrVal);
    funcDef.retType = langTypeToIRType(funcType->ft.retType);

    ListNode* params = fdn->params;
    for (size_t i = 0; i < params->chLen; i++)
    {
        FuncParamNode* paramNode = params->children[i].fpn;

        Symbol* paramSymb =
            GetSymbolByNameFromScope(this->GetCurrScope(), paramNode->paramName->StrVal);

        auto param = this->newRegister(langTypeToIRType(paramSymb->type));

        funcDef.params.push_back(param);
        this->AddVar(paramSymb, param);
    }
}

void IRCompiler::walkRoot()
{

    ListNode* funcsList = this->astRoot.ln;

    for (size_t i = 0; i < funcsList->chLen; i++)
    {

        this->currFunc = new IR::Function();
        this->nextId   = 0; // resetting id's for new function

        FuncDeclNode* fdn = funcsList->children[i].fdn;

        this->funcs[std::string(fdn->fName->StrVal)] =
            std::unique_ptr<IR::Function>(this->currFunc);

        this->nextCtx();
        this->buildFuncDef(fdn);
        this->walkFuncBody(fdn->body);
        this->prevCtx();
    }
}

void IRCompiler::RunAstToIRCompiler()
{
    this->ctxStack.push(WalkerCtx(this->rootScope));
    this->walkRoot();
}
