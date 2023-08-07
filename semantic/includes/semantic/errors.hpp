/** @file */
#ifndef SEMANTIC_ERRORS_PUBLIC_HPP
#define SEMANTIC_ERRORS_PUBLIC_HPP

#include "parser/parser.hpp"
#include "semantic/symtab.hpp"
#include "semantic/types.hpp"

enum SemErrType
{
    SEM_ERR_NO_ERR,

    SEM_ERR_UNDEF_SYMB,
    SEM_ERR_SYMB_REDEF,
    SEM_ERR_OPERATION_UNDEF,
    SEM_ERR_SYMB_UNCALLABLE,
    SEM_ERR_FUNC_CALL_NE_ARGS,
    SEM_ERR_FUNC_CALL_ARG_MISMATCH,
    SEM_ERR_BAD_RET_TYPE,
    SEM_ERR_JMP_OUTSIDE_LOOP,
    SEM_ERR_TYPES_MISMATCH,
    SEM_ERR_COND_BAD_TYPE,

};

struct SemUndSymbErr
{
    Token* symbol;
};

struct SemSymRedefErr
{
    Token*  symbol;
    Symbol* def;
};

struct SemUndOpErr
{
    Token*      op;
    const Type* forType;
};

struct SemUncallSymbErr
{
    Symbol* symb;
};

struct SemFuncInvArgsCount
{
    Symbol* func;
    size_t  argsSz;
    Token*  fCall;
};

struct SemFuncArgMismatch
{
    Symbol*     funcSymbol;
    const Type* argType;
    size_t      argIdx;
    Token*      fCall;
};

struct SemBadRetType
{
    Token*      retTok;
    const Type* ret;
    const Type* retDesired;
};

struct SemJmpOutLoop
{
    Token* jmp;
};

struct SemTypesMismatch
{
    Token*      op;
    const Type* t1;
    const Type* t2;
};

struct SemCondBadType
{
    Token*      cond;
    const Type* condType;
};

struct SemanticError // TODO: split into multiple struct
{
    SemErrType type;

    union
    {
        SemUndSymbErr       undsym;
        SemSymRedefErr      symred;
        SemUndOpErr         undop;
        SemUncallSymbErr    uncsym;
        SemFuncInvArgsCount invarg;
        SemFuncArgMismatch  argmsmtch;
        SemBadRetType       badret;
        SemJmpOutLoop       badjmp;
        SemTypesMismatch    typmsmtch;
        SemCondBadType      condbad;
    };
};

void PrintError(SemanticError* err, FILE* out);

#endif
