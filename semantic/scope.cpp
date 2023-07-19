#include "scope.hpp"

#include <stdlib.h>

#include "symtab.hpp"

ScopeNode* NewScopeNode()
{
    ScopeNode* n = (ScopeNode*)calloc(1, sizeof(ScopeNode));

    return n;
}

void AddNewScope(ScopeNode* prev, ScopeNode* newScope)
{
    if (newScope == NULL)
        return;

    if (prev->chCap == 0)
    {
        prev->chCap    = 1;
        prev->children = (ScopeNode**)calloc(prev->chCap, sizeof(ScopeNode*));
    }

    if (prev->chCap == prev->chLen)
    {
        prev->chCap *= 2;
        prev->children = (ScopeNode**)realloc(prev->children, sizeof(ScopeNode*) * prev->chCap);
    }

    newScope->prev              = prev;
    prev->children[prev->chLen] = newScope;

    prev->chLen++;
}

Symbol* GetSymbolByNameFromScope(ScopeNode* sc, const char* name)
{
    Symbol* res = NULL;

    ScopeNode* currScope = sc;
    while (currScope != NULL)
    {
        res = GetSymbolByName(&currScope->map, name);
        if (res != NULL)
            return res;

        currScope = currScope->prev;
    }

    return NULL;
}

Type* GetTypeByNameFromScope(ScopeNode* sc, const char* tname)
{
    return GetSymbolByNameFromScope(sc, tname)->type;
}

static Type* getTypeByName(ScopeNode* curr, Token* typeNameTok)
{
    const char* typeName = NULL;
    if (typeNameTok->n != ID)
    {
        typeName = TokenNameToLexeme(typeNameTok->n); // base type name
    }

    Symbol* ts = GetSymbolByNameFromScope(curr, typeName);
    if (ts == NULL)
        return NULL;

    if (ts->sclass != SYM_TYPE_DEF)
    {
        return NULL;
    }

    return ts->type;
}

Symbol* NewVarDef(ScopeNode* curr, Token* varTypeTok, Token* varNameTok)
{
    Symbol* dup = GetSymbolByName(&curr->map, varNameTok->StrVal);
    if (dup != NULL)
        return dup;

    Type* varT = getTypeByName(curr, varTypeTok);
    if (varT == NULL)
    {
        printf("unreachable for now"); // TODO
        return NULL;
    }

    Symbol* vds = (Symbol*)calloc(1, sizeof(Symbol));

    *vds = Symbol{
        .sclass = SYM_VAR,
        .type   = varT,
        .tok    = varNameTok,
        .name   = varNameTok->StrVal,
    };

    return AddNewSymbol(&curr->map, vds);
}

static Type* buildFuncType(ScopeNode* curr, FuncDeclNode* fdn)
{
    Type* t   = (Type*)calloc(1, sizeof(Type));
    t->tclass = TypeClassFunc;

    ListNode* params = fdn->params;

    FuncType* fncType = &t->ft;
    for (size_t i = 0; i < params->chLen; i++)
    {
        FuncParamNode* fpn = params->children[i].fpn;

        Type* paramT = getTypeByName(curr, fpn->typeName);
        if (paramT == NULL)
        {
            printf("unreachable for now"); // TODO
            free(t);
            return NULL;
        }

        fncType->paramTypes[fncType->paramsSz] = paramT;
        fncType->paramsSz++;
    }

    Type* retT = getTypeByName(curr, fdn->retTypeName);
    if (retT == NULL)
    {
        printf("unreachable for now"); // TODO
        free(t);
        return NULL;
    }
    fncType->retType = retT;

    return t;
}

Symbol* NewFuncDef(ScopeNode* curr, FuncDeclNode* fdn)
{

    Symbol* dup = GetSymbolByName(&curr->map, fdn->fName->StrVal);
    if (dup != NULL)
        return dup;

    Type* fType = buildFuncType(curr, fdn);

    Symbol* fdef = (Symbol*)calloc(1, sizeof(Symbol));

    *fdef = Symbol{
        .sclass = SYM_FUNC,
        .type   = fType,
        .tok    = fdn->fName,
        .name   = fdn->fName->StrVal,
    };

    return AddNewSymbol(&curr->map, fdef);
}

Type* DefineBaseType(ScopeNode* curr, BaseType bt, const char* name)
{

    Type* newType   = (Type*)calloc(1, sizeof(Type));
    newType->tclass = TypeClassBase;
    newType->bt     = bt;
    newType->name   = name;

    Symbol* bts = (Symbol*)calloc(1, sizeof(Symbol));

    *bts = {
        .sclass = SYM_TYPE_DEF,
        .type   = newType,
        .name   = name,
    };

    AddNewSymbol(&curr->map, bts);

    return newType;
}

void FreeScope(ScopeNode* sc)
{

    FreeSymTable(&sc->map);

    for (size_t i = 0; i < sc->chLen; i++)
        FreeScope(sc->children[i]);

    free(sc->children);
    free(sc);
}
