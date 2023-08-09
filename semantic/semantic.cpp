
#include <stdlib.h>

#include "errors.hpp"
#include "scope.hpp"
#include "semantic/checker.hpp"

const Type* intTypeObj;
const Type* floatTypeObj;
const Type* boolTypeObj;

static bool areEqTypes(const Type* t1, const Type* t2)
{
    if (t1 == NULL || t2 == NULL)
        return false;

    if (t1->tclass != TypeClassBase || t2->tclass != TypeClassBase) // TODO: complex types
    {
        return false;
    }

    return t1->bt == t2->bt;
}

static const Type* checkSimpleStmt(SemanticChecker* sch, ScopeNode* curr, Node n);

static void walkInnerScope(SemanticChecker* sch, ScopeNode* curr, Node n);

static const Type* checkValue(SemanticChecker* sch, ScopeNode* curr, ValNode* v)
{
    TokenName valType = v->val->n;
    switch (valType)
    {
        case TOK_TRUE:
        case TOK_FALSE:
            return boolTypeObj;

        case TOK_Integer:
            return intTypeObj;
        case TOK_Float:
            return floatTypeObj;

        case TOK_ID:
        {
            Symbol* sym = GetSymbolByNameFromScope(curr, v->val->StrVal);
            if (sym == NULL)
            {
                AddNewErr(sch, NewUndefSymbErr(v->val));
                return NULL;
            }
            return sym->type;
        }

        default:
            return NULL;
    }
}

static const Type* checkUnOp(SemanticChecker* sch, ScopeNode* curr, UnOpNode* uopn)
{
    const Type* chType = checkSimpleStmt(sch, curr, uopn->child);
    if (chType == NULL)
        return NULL;

    TokenName op = uopn->op->n;

    if (op & TOK_NOT)
    {
        if (!areEqTypes(chType, boolTypeObj))
            AddNewErr(sch, NewUndefOpErr(uopn->op, chType));

        return chType;
    }

    if (op & TOK_MINUS)
    {
        if (!areEqTypes(chType, intTypeObj) && !areEqTypes(chType, floatTypeObj))
            AddNewErr(sch, NewUndefOpErr(uopn->op, chType));

        return chType;
    }

    if (op & TOK_RETURN)
    {
        const Type* currFuncRetType =
            GetSymbolByNameFromScope(curr, sch->currFunc->fName->StrVal)->type->ft.retType;

        if (!areEqTypes(chType, currFuncRetType))
            AddNewErr(sch, NewBadRetType(chType, currFuncRetType, uopn->op));

        return NULL;
    }

    if (op & (TOK_BREAK | TOK_CONTINUE))
    {
        if (sch->currCycle == NULL)
            AddNewErr(sch, NewJmpOutsideLoopErr(uopn->op));

        return NULL;
    }

    return NULL;
}

static const Type* checkBinOp(SemanticChecker* sch, ScopeNode* curr, BinOpNode* bopn)
{
    const Type* lType = checkSimpleStmt(sch, curr, bopn->left);
    const Type* rType = checkSimpleStmt(sch, curr, bopn->right);

    if (lType == NULL || rType == NULL)
        return NULL;

    if (!areEqTypes(lType, rType))
    {
        AddNewErr(sch, NewTypesMismatchErr(lType, rType, bopn->op));
        return NULL;
    }

    TokenName op = bopn->op->n;

    if (op & TOK_ASSIGN)
    {
        return NULL;
    }

    if (op & (TOK_LAND | TOK_LOR))
    {
        if (!areEqTypes(lType, boolTypeObj))
        {
            AddNewErr(sch, NewUndefOpErr(bopn->op, lType));
            return NULL;
        }

        return boolTypeObj;
    }

    if (op & (TOK_MULT | TOK_DIV | TOK_PLUS | TOK_MINUS | TOK_GE | TOK_GT | TOK_LE | TOK_LT))
    {
        if (!areEqTypes(lType, intTypeObj) && !areEqTypes(lType, floatTypeObj))
        {
            AddNewErr(sch, NewUndefOpErr(bopn->op, lType));
            return NULL;
        }

        if (op & (TOK_MULT | TOK_DIV | TOK_PLUS | TOK_MINUS))
            return lType;

        return boolTypeObj;
    }

    if (op & (TOK_EQ | TOK_NE))
    {
        return boolTypeObj;
    }

    printf("unreachable");
    return NULL;
}

static void checkVarDecl(SemanticChecker* sch, ScopeNode* curr, VarDeclNode* vdn)
{
    if (vdn->initVal.hdr.type != NODE_EMPTY)
    {
        const Type* initType = checkSimpleStmt(sch, curr, vdn->initVal);
        const Type* varType  = GetTypeByNameFromScope(curr, TokenNameToLexeme(vdn->typeName->n));

        if (varType == NULL)
            return;

        if (initType && !areEqTypes(initType, varType))
            AddNewErr(sch, NewTypesMismatchErr(varType, initType, vdn->varName));
    }

    Symbol* def = NewVarDef(curr, vdn->typeName, vdn->varName);
    if (def != NULL)
        AddNewErr(sch, NewSymbRedefErr(vdn->varName, def));
}

static const Type* checkFuncCall(SemanticChecker* sch, ScopeNode* curr, FuncCallNode* fcn)
{

    Symbol* funcSymb = GetSymbolByNameFromScope(curr, fcn->fName->StrVal);
    if (funcSymb == NULL)
    {
        AddNewErr(sch, NewUndefSymbErr(fcn->fName));
        return NULL;
    }

    if (funcSymb->sclass != SYM_FUNC)
    {
        AddNewErr(sch, NewUncallableSymbErr(funcSymb));
        return NULL;
    }

    FuncType ft = funcSymb->type->ft;

    ListNode*    args   = fcn->args;
    const Type** params = ft.paramTypes;

    if (args->chLen != funcSymb->type->ft.paramsSz)
    {
        AddNewErr(sch, NewBadFuncCallNEArgs(funcSymb, fcn->fName, args->chLen));
        return ft.retType;
    }

    for (size_t i = 0; i < args->chLen; i++)
    {
        const Type* currParamType = params[i];

        const Type* currArgType = checkSimpleStmt(sch, curr, args->children[i]);
        if (currArgType == NULL)
            continue;

        if (!areEqTypes(currArgType, currParamType))
            AddNewErr(sch, NewFuncArgMismatch(funcSymb, fcn->fName, currArgType, i));
    }

    return ft.retType;
}

static void walkBlock(SemanticChecker* sch, ScopeNode* curr, ListNode* ln)
{

    for (size_t i = 0; i < ln->chLen; i++)
    {
        Node next = ln->children[i];

        NodeType nt = next.hdr.type;

        if (nt == NODE_VAR_DECL)
            checkVarDecl(sch, curr, next.vdn);

        else if (nt == NODE_FUNCTION_CALL)
            checkFuncCall(sch, curr, next.fcn);

        else if (nt == NODE_BINOP)
            checkBinOp(sch, curr, next.bopn);

        else if (nt == NODE_UNOP)
            checkUnOp(sch, curr, next.uopn);

        else
            walkInnerScope(sch, curr, next);
    }
}

static void walkIfStmt(SemanticChecker* sch, ScopeNode* curr, IfStmtNode* ifstn)
{

    const Type* condType = checkSimpleStmt(sch, curr, ifstn->cond);

    if (condType != NULL && !areEqTypes(condType, boolTypeObj))
        AddNewErr(sch, NewBadCondTypeErr(ifstn->ifTok, condType));

    walkBlock(sch, curr, ifstn->body);
    walkInnerScope(sch, curr, ifstn->elseBody);
}

static void walkForStmt(SemanticChecker* sch, ScopeNode* curr, ForStmtNode* forstn)
{

    const Type* condType = checkSimpleStmt(sch, curr, forstn->cond);
    if (condType != NULL && !areEqTypes(condType, boolTypeObj))
        AddNewErr(sch, NewBadCondTypeErr(forstn->forTok, condType));

    walkBlock(sch, curr, forstn->body);
}

static const Type* checkSimpleStmt(SemanticChecker* sch, ScopeNode* curr, Node n)
{

    switch (n.hdr.type)
    {
        case NODE_FUNCTION_CALL:
            return checkFuncCall(sch, curr, n.fcn);

        case NODE_BINOP:
            return checkBinOp(sch, curr, n.bopn);

        case NODE_UNOP:
            return checkUnOp(sch, curr, n.uopn);

        case NODE_VAL:
            return checkValue(sch, curr, n.vn);

        default:
            return NULL;
    }
}

static void walkInnerScope(SemanticChecker* sch, ScopeNode* curr, Node n)
{
    switch (n.hdr.type)
    {

        case NODE_COMPOUND_STMT:
        {
            ScopeNode* nextScope = NewScopeNode();
            AddNewScope(curr, nextScope);

            walkBlock(sch, nextScope, n.ln);
            break;
        }

        case NODE_IF_STMT:
        {
            ScopeNode* nextScope = NewScopeNode();
            AddNewScope(curr, nextScope);

            walkIfStmt(sch, nextScope, n.ifstn);
            break;
        }

        case NODE_FOR_STMT:
        {
            ScopeNode* nextScope = NewScopeNode();
            AddNewScope(curr, nextScope);

            sch->currCycle = n.forstn;

            walkForStmt(sch, nextScope, n.forstn);

            sch->currCycle = NULL;
            break;
        }

        default:
            break;
    }
}

static void walkFunctionScope(SemanticChecker* sch, ScopeNode* curr, FuncDeclNode* fdn)
{
    sch->currFunc = fdn;

    ListNode* params = fdn->params;
    for (size_t i = 0; i < params->chLen; i++)
    {
        FuncParamNode* currParam = params->children[i].fpn;

        Symbol* def = NewVarDef(curr, currParam->typeName, currParam->paramName);
        if (def != NULL)
            AddNewErr(sch, NewSymbRedefErr(currParam->paramName, def));
    }

    walkBlock(sch, curr, fdn->body);
}

static void walkGlobalScope(SemanticChecker* sch, ListNode* programm)
{
    for (size_t i = 0; i < programm->chLen; i++)
    {
        FuncDeclNode* currFunc = programm->children[i].fdn;

        Symbol* redef = NewFuncDef(sch->rootScope, currFunc);
        if (redef != NULL)
            AddNewErr(sch, NewSymbRedefErr(currFunc->fName, redef));
    }

    for (size_t i = 0; i < programm->chLen; i++)
    {
        ScopeNode* nextScope = NewScopeNode();
        AddNewScope(sch->rootScope, nextScope);

        walkFunctionScope(sch, nextScope, programm->children[i].fdn);
    }
}

void RunSemCheck(SemanticChecker* sch, ListNode* programm)
{

    sch->rootScope = NewScopeNode();

    intTypeObj = DefineBaseType(sch->rootScope, BaseTypeInteger, TokenNameToLexeme(TOK_IntType));
    floatTypeObj =
        DefineBaseType(sch->rootScope, BaseTypeFloat64, TokenNameToLexeme(TOK_FloatType));
    boolTypeObj = DefineBaseType(sch->rootScope, BaseTypeBoolean, TokenNameToLexeme(TOK_BoolType));

    walkGlobalScope(sch, programm);
}

void FreeSemCheck(SemanticChecker* sch)
{
    FreeScope(sch->rootScope);
    free(sch->errors);
}
