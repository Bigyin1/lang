#include "errors.hpp"

#include <stdio.h>
#include <stdlib.h>

SemanticError NewUndefSymbErr(Token* symbol)
{

    return SemanticError{
        .type = SEM_ERR_UNDEF_SYMB,
        .undsym =
            SemUndSymbErr{
                .symbol = symbol,
            },
    };
}

static void PrintError(SemUndSymbErr* err, FILE* out)
{
    fprintf(out, "undefined symbol \"%s\" at %u, %u\n", err->symbol->StrVal, err->symbol->Row,
            err->symbol->Col);
}

SemanticError NewSymbRedefErr(Token* symbol, Symbol* def)
{
    return SemanticError{
        .type = SEM_ERR_SYMB_REDEF,
        .symred =
            SemSymRedefErr{
                .symbol = symbol,
                .def    = def,
            },
    };
}

static void PrintError(SemSymRedefErr* err, FILE* out)
{
    fprintf(out, "symbol \"%s\" redefined at %u, %u; firstly defined at %u, %u\n",
            err->symbol->StrVal, err->symbol->Row, err->symbol->Col, err->def->tok->Row,
            err->def->tok->Col);
}

SemanticError NewUndefOpErr(Token* op, const Type* t)
{

    return SemanticError{
        .type = SEM_ERR_OPERATION_UNDEF,
        .undop =
            SemUndOpErr{
                .op      = op,
                .forType = t,
            },
    };
}

static void PrintError(SemUndOpErr* err, FILE* out)
{
    fprintf(out, "operation \"%s\" is undefined for type \"%s\" %u, %u\n",
            TokenNameToLexeme(err->op->n), err->forType->name, err->op->Row, err->op->Col);
}

SemanticError NewUncallableSymbErr(Symbol* symbol)
{
    return SemanticError{
        .type = SEM_ERR_SYMB_UNCALLABLE,
        .uncsym =
            SemUncallSymbErr{
                .symb = symbol,
            },
    };
}

static void PrintError(SemUncallSymbErr* err, FILE* out)
{
    fprintf(out, "symbol \"%s\" is uncallable %u, %u\n", err->symb->name, err->symb->tok->Row,
            err->symb->tok->Col);
}

SemanticError NewBadFuncCallNEArgs(Symbol* funcSymb, Token* fCall, size_t argsSz)
{
    return SemanticError{
        .type = SEM_ERR_FUNC_CALL_NE_ARGS,
        .invarg =
            SemFuncInvArgsCount{
                .func   = funcSymb,
                .argsSz = argsSz,
                .fCall  = fCall,
            },
    };
}

static void PrintError(SemFuncInvArgsCount* err, FILE* out)
{
    fprintf(out, "function \"%s\" call has %zu arguments, expected %zu; %u, %u\n", err->func->name,
            err->argsSz, err->func->type->ft.paramsSz, err->fCall->Row, err->fCall->Col);
}

SemanticError NewFuncArgMismatch(Symbol* funcSymb, Token* fCall, const Type* argT, size_t argPos)
{
    return SemanticError{
        .type = SEM_ERR_FUNC_CALL_ARG_MISMATCH,
        .argmsmtch =
            SemFuncArgMismatch{
                .funcSymbol = funcSymb,
                .argType    = argT,
                .argIdx     = argPos,
                .fCall      = fCall,
            },
    };
}

static void PrintError(SemFuncArgMismatch* err, FILE* out)
{
    fprintf(out,
            "function \"%s\" call has argument №%zu of type \"%s\", expected type: \"%s\"; "
            "%u, %u\n",
            err->funcSymbol->name, err->argIdx + 1, err->argType->name,
            err->funcSymbol->type->ft.paramTypes[err->argIdx]->name, err->fCall->Row,
            err->fCall->Col);
}

SemanticError NewBadRetType(const Type* ret, const Type* wanted, Token* retTok)
{
    return SemanticError{
        .type = SEM_ERR_BAD_RET_TYPE,
        .badret =
            SemBadRetType{
                .retTok     = retTok,
                .ret        = ret,
                .retDesired = wanted,
            },
    };
}

static void PrintError(SemBadRetType* err, FILE* out)
{
    fprintf(out,
            "return type: \"%s\"; expected type: \"%s\"; "
            "%u, %u\n",
            err->ret->name, err->retDesired->name, err->retTok->Row, err->retTok->Col);
}

SemanticError NewJmpOutsideLoopErr(Token* jmp)
{
    return SemanticError{
        .type = SEM_ERR_JMP_OUTSIDE_LOOP,
        .badjmp =
            SemJmpOutLoop{
                .jmp = jmp,
            },
    };
}

static void PrintError(SemJmpOutLoop* err, FILE* out)
{
    fprintf(out,
            "\"%s\" is outside loop statement; "
            "%u, %u\n",
            TokenNameToLexeme(err->jmp->n), err->jmp->Row, err->jmp->Col);
}

SemanticError NewTypesMismatchErr(const Type* t1, const Type* t2, Token* op)
{

    return SemanticError{
        .type = SEM_ERR_TYPES_MISMATCH,
        .typmsmtch =
            SemTypesMismatch{
                .op = op,
                .t1 = t1,
                .t2 = t2,
            },
    };
}

static void PrintError(SemTypesMismatch* err, FILE* out)
{
    fprintf(out,
            "types mismatch (\"%s\" and \"%s\") for operation at: "
            "%u, %u\n",
            err->t1->name, err->t2->name, err->op->Row, err->op->Col);
}

SemanticError NewBadCondTypeErr(Token* cond, const Type* condType)
{

    return SemanticError{
        .type = SEM_ERR_COND_BAD_TYPE,
        .condbad =
            SemCondBadType{
                .cond     = cond,
                .condType = condType,
            },
    };
}

static void PrintError(SemCondBadType* err, FILE* out)
{
    fprintf(out,
            "condition must be of boolean type, got type: \"%s\"; "
            "%u, %u\n",
            err->condType->name, err->cond->Row, err->cond->Col);
}

void AddNewErr(SemanticChecker* sch, SemanticError err)
{

    if (sch->errsCap == 0)
    {
        sch->errsCap = 1;
        sch->errors  = (SemanticError*)calloc(sch->errsCap, sizeof(SemanticError));
    }

    if (sch->errsCap == sch->errsSz)
    {
        sch->errsCap *= 2;
        sch->errors = (SemanticError*)realloc(sch->errors, sizeof(SemanticError) * sch->errsCap);
    }

    sch->errors[sch->errsSz] = err;
    sch->errsSz++;
}

void PrintError(SemanticError* err, FILE* out)
{

    switch (err->type)
    {
        case SEM_ERR_NO_ERR:
            break;

        case SEM_ERR_UNDEF_SYMB:
            return PrintError(&err->undsym, out);

        case SEM_ERR_SYMB_REDEF:
            return PrintError(&err->symred, out);

        case SEM_ERR_OPERATION_UNDEF:
            return PrintError(&err->undop, out);

        case SEM_ERR_SYMB_UNCALLABLE:
            return PrintError(&err->uncsym, out);

        case SEM_ERR_FUNC_CALL_NE_ARGS:
            return PrintError(&err->invarg, out);

        case SEM_ERR_FUNC_CALL_ARG_MISMATCH:
            return PrintError(&err->argmsmtch, out);

        case SEM_ERR_BAD_RET_TYPE:
            return PrintError(&err->badret, out);

        case SEM_ERR_JMP_OUTSIDE_LOOP:
            return PrintError(&err->badjmp, out);

        case SEM_ERR_TYPES_MISMATCH:
            return PrintError(&err->typmsmtch, out);

        case SEM_ERR_COND_BAD_TYPE:
            return PrintError(&err->condbad, out);

        default:
            break;
    }
}
