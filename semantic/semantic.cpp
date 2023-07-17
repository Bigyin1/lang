
#include "scope.hpp"
#include "semantic/checker.hpp"

// static bool typesAreEqual(Token* typeName1, Token* typeName2)
// {

//     if (typeName1->n == typeName2->n)
//         return true;

//     return false;

// } // TODO: complex types

static TokenName checkSimpleStmt(SemanticChecker* sch, ScopeNode* curr, Node n);

static void walkInnerScope(SemanticChecker* sch, ScopeNode* curr, Node n);

static TokenName checkValue(SemanticChecker* sch, ScopeNode* curr, ValNode* v)
{
    switch (v->val->n)
    {
        case TRUE:
        case FALSE:
            return BoolType;

        case Integer:
            return IntType;
        case Float:
            return FloatType;

        case ID:
        {
            Symbol* sym = GetSymbolByNameFromScope(curr, v->val->StrVal);
            if (sym == NULL)
            {
                printf("undefined variable: %s; row %u, col: %u\n", v->val->StrVal, v->val->Row,
                       v->val->Col);
                return TokEOF;
            }
            return sym->Type;
        }

        default:
            break;
    }
}

static TokenName checkUnOp(SemanticChecker* sch, ScopeNode* curr, UnOpNode* uopn)
{
    TokenName chType = checkSimpleStmt(sch, curr, uopn->child);
    if (uopn->child.hdr.type != NODE_EMPTY && chType == TokEOF)
        return TokEOF;

    TokenName op = uopn->op->n;

    if (op & NOT)
    {
        if (chType != BoolType)
        {
            printf("boolean type wanted for %s opeartion; row %u, col: %u\n", TokenNameToString(op),
                   uopn->op->Row, uopn->op->Col);
            return TokEOF;
        }
    }

    if (op & MINUS)
    {
        if (!(chType & (IntType | FloatType)))
        {
            printf("int or float type wanted for %s opeartion; row %u, col: %u\n",
                   TokenNameToString(op), uopn->op->Row, uopn->op->Col);
            return TokEOF;
        }
    }

    if (op & RETURN)
    {
        if (chType != sch->currFunc->retTypeName->n)
        {
            printf("retrun stmt at line %u: wrong type: %s, must return %s\n", uopn->op->Row,
                   TokenNameToString(chType), TokenNameToString(sch->currFunc->retTypeName->n));
        }
    }

    if (op & (BREAK | CONTINUE))
    {
        if (sch->currCycle == NULL)
        {
            printf("%s at line %u is outside any cycle block\n", TokenNameToString(op),
                   uopn->op->Row);
        }
    }

    return TokEOF;
}

static TokenName checkBinOp(SemanticChecker* sch, ScopeNode* curr, BinOpNode* bopn)
{

    TokenName lType = checkSimpleStmt(sch, curr, bopn->left);
    TokenName rType = checkSimpleStmt(sch, curr, bopn->right);

    if (lType == TokEOF || rType == TokEOF)
        return TokEOF;

    if (lType != rType)
    {
        printf("arguments types mismatch for %s; row %u, col: %u\n", TokenNameToString(bopn->op->n),
               bopn->op->Row, bopn->op->Col);
        return TokEOF;
    }

    TokenName op = bopn->op->n;

    if (op & (LAND | LOR))
    {
        if (lType != BoolType)
        {
            printf("boolean type wanted for %s opeartion; row %u, col: %u\n",
                   TokenNameToString(bopn->op->n), bopn->op->Row, bopn->op->Col);
            return TokEOF;
        }

        return BoolType;
    }

    if (op & (MULT | DIV | PLUS | MINUS))
    {
        if (!(lType & (IntType | FloatType)))
        {
            printf("int or float type wanted for %s opeartion; row %u, col: %u\n",
                   TokenNameToString(bopn->op->n), bopn->op->Row, bopn->op->Col);
            return TokEOF;
        }

        return lType;
    }
    else if (op & (EQ | GE | GT | LE | LT | NE))
    {
        if (!(lType & (IntType | FloatType)))
        {
            printf("int or float type wanted for %s opeartion; row %u, col: %u\n",
                   TokenNameToString(bopn->op->n), bopn->op->Row, bopn->op->Col);
            return TokEOF;
        }

        return BoolType;
    }

    printf("unreachable");
    return TokEOF;
}

static void checkVarDecl(SemanticChecker* sch, ScopeNode* curr, VarDeclNode* vdn)
{

    if (vdn->initVal.hdr.type != NODE_EMPTY)
    {
        TokenName initType = checkSimpleStmt(sch, curr, vdn->initVal);
        if (initType != TokEOF && initType != vdn->typeName->n)
        {
            printf("variable %s initialisation types mismatch: wanted type: %s; init type: %s; row "
                   "%u, col: %u\n",
                   vdn->varName->StrVal, TokenNameToString(vdn->typeName->n),
                   TokenNameToString(initType), vdn->varName->Row, vdn->varName->Col);
        }
    }

    Symbol var = {
        .Type = vdn->typeName->n,
        .Tok  = vdn->varName,
        .Name = vdn->varName->StrVal,
    };

    Symbol* redef = AddNewSymbol(&curr->map, var);
    if (redef != NULL)
    {
        printf("variable %s redefenition on row: %u, col: %u; defined on row: %u, col: %u\n",
               vdn->varName->StrVal, vdn->varName->Row, vdn->varName->Col, redef->Tok->Row,
               redef->Tok->Col);
    }
}

static TokenName checkFuncCall(SemanticChecker* sch, ScopeNode* curr, FuncCallNode* fcn)
{

    Symbol* sym = GetSymbolByNameFromScope(curr, fcn->fName->StrVal);
    if (sym == NULL)
    {
        printf("undefined function: %s; row %u, col: %u\n", fcn->fName->StrVal, fcn->fName->Row,
               fcn->fName->Col);
        return TokEOF;
    }

    if (sym->FuncType == NULL)
    {
        printf("unable to call varibale %s; row %u, col: %u\n", fcn->fName->StrVal, fcn->fName->Row,
               fcn->fName->Col);

        return TokEOF;
    }

    ListNode* args   = fcn->args;
    ListNode* params = sym->FuncType->params;

    if (args->chLen != params->chLen)
    {
        printf("wrong arguments count in function %s call; row %u, col: %u\n", fcn->fName->StrVal,
               fcn->fName->Row, fcn->fName->Col);
        return sym->FuncType->retTypeName->n;
    }

    for (size_t i = 0; i < args->chLen; i++)
    {
        FuncParamNode* currParam = params->children[i].fpn;

        TokenName argType = checkSimpleStmt(sch, curr, args->children[i]);
        if (argType == TokEOF)
            continue;

        if (argType != currParam->typeName->n)
        {
            printf("function %s call type mismatch in argument %zu; row %u, col: %u\n",
                   fcn->fName->StrVal, i + 1, fcn->fName->Row, fcn->fName->Col);
        }
    }

    return sym->FuncType->retTypeName->n;
}

static void walkBlock(SemanticChecker* sch, ScopeNode* curr, ListNode* ln)
{

    for (size_t i = 0; i < ln->chLen; i++)
    {
        Node next = ln->children[i];

        if (next.hdr.type == NODE_VAR_DECL)
        {
            checkVarDecl(sch, curr, next.vdn);
            continue;
        }
        if (checkSimpleStmt(sch, curr, next) != TokEOF)
            continue;

        walkInnerScope(sch, curr, next);
    }
}

static void walkIfStmt(SemanticChecker* sch, ScopeNode* curr, IfStmtNode* ifstn)
{

    TokenName condType = checkSimpleStmt(sch, curr, ifstn->cond);
    if (condType != TokEOF && condType != BoolType)
    {
        printf("condition in IF statement must be boolean; row: %u, col: %u\n", ifstn->ifTok->Row,
               ifstn->ifTok->Col);
    }

    walkBlock(sch, curr, ifstn->body);
    walkInnerScope(sch, curr, ifstn->elseBody);
}

static void walkForStmt(SemanticChecker* sch, ScopeNode* curr, ForStmtNode* forstn)
{

    TokenName condType = checkSimpleStmt(sch, curr, forstn->cond);
    if (condType != TokEOF && condType != BoolType)
    {
        printf("condition in IF statement must be boolean; row: %u, col: %u\n", forstn->forTok->Row,
               forstn->forTok->Col);
    }

    walkBlock(sch, curr, forstn->body);
}

static TokenName checkSimpleStmt(SemanticChecker* sch, ScopeNode* curr, Node n)
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
            return TokEOF;
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

        Symbol glob = {
            .Type = currParam->typeName->n,
            .Tok  = currParam->paramName,
            .Name = currParam->paramName->StrVal,
        };

        Symbol* redef = AddNewSymbol(&curr->map, glob);
        if (redef != NULL)
        {
            printf(
                "parameter %s duplicate on row: %u, col: %u; firstly defined on row: %u, col: %u\n",
                currParam->paramName->StrVal, currParam->paramName->Row, currParam->paramName->Col,
                redef->Tok->Row, redef->Tok->Col);
        }
    }

    walkBlock(sch, curr, fdn->body);
}

static ScopeNode* walkGlobalScope(SemanticChecker* sch, ListNode* programm)
{
    ScopeNode* currScope = NewScopeNode();

    for (size_t i = 0; i < programm->chLen; i++)
    {
        FuncDeclNode* currFunc = programm->children[i].fdn;

        Symbol glob = {
            .FuncType = currFunc,
            .Tok      = currFunc->fName,
            .Name     = currFunc->fName->StrVal,
        };

        Symbol* redef = AddNewSymbol(&currScope->map, glob);
        if (redef != NULL)
        {
            printf("function %s duplicate on row: %u, col: %u\n", currFunc->fName->StrVal,
                   currFunc->fName->Row, currFunc->fName->Col);
        }
    }

    for (size_t i = 0; i < programm->chLen; i++)
    {
        ScopeNode* nextScope = NewScopeNode();
        AddNewScope(currScope, nextScope);

        walkFunctionScope(sch, nextScope, programm->children[i].fdn);
    }

    return currScope;
}

void RunSemCheck(SemanticChecker* sch, ListNode* programm)
{

    sch->rootScope = walkGlobalScope(sch, programm);
}
