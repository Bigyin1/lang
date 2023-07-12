#include "parser/grVizGen.hpp"

#include "lexer/lexemes.hpp"

static const char* header = "digraph expr {"
                            "fontname=\"Helvetica,Arial,sans-serif\""
                            "node [fontname=\"Helvetica,Arial,sans-serif\"]"
                            "edge [fontname=\"Helvetica,Arial,sans-serif\"]"
                            "node [shape=box];";

static void walkNode(Node n, FILE* out);

static void walkListNode(ListNode* ln, FILE* out, const char* name)
{

    fprintf(out, "\nN%p[\n label=<%s>\n]", ln, name);

    for (size_t i = 0; i < ln->chLen; i++)
    {
        walkNode(ln->children[i], out);
    }

    for (size_t i = 0; i < ln->chLen; i++)
    {
        fprintf(out, "\nN%p -> N%p", ln, ln->children[i].gen);
    }
}

static void walkFuncDecl(FuncDeclNode* fd, FILE* out)
{

    fprintf(out,
            "\nN%p[\nshape=record\n"
            "label=<func | %s | %s>\n]",
            fd, fd->fName->StrVal, TokenNameToString(fd->retTypeName->n));

    walkListNode(fd->params, out, "Parameters");
    walkListNode(fd->body, out, "Body");

    fprintf(out, "\nN%p -> N%p", fd, fd->params);
    fprintf(out, "\nN%p -> N%p", fd, fd->body);
}

static void walkFuncParam(FuncParamNode* pn, FILE* out)
{
    fprintf(out,
            "\nN%p[\nshape=record\n"
            "label=<%s | %s>\n]",
            pn, pn->paramName->StrVal, TokenNameToString(pn->typeName->n));
}

static void walkFuncCall(FuncCallNode* fcn, FILE* out)
{

    fprintf(out, "\nN%p[\nshape=record\n label=<call | %s>\n]", fcn, fcn->fName->StrVal);

    walkListNode(fcn->args, out, "Arguments");

    fprintf(out, "\nN%p -> N%p", fcn, fcn->args);
}

static void walkVarDecl(VarDeclNode* vdn, FILE* out)
{

    fprintf(out,
            "\nN%p[\nshape=record\n"
            "label=<%s | %s>\n]",
            vdn, vdn->varName->StrVal, TokenNameToString(vdn->typeName->n));

    walkNode(vdn->initVal, out);

    fprintf(out, "\nN%p -> N%p", vdn, vdn->initVal.gen);
}

static void walkValue(ValNode* vn, FILE* out)
{

    fprintf(out,
            "\nN%p[\n"
            "label=<",
            vn);

    switch (vn->val->n)
    {
        case ID:
            fprintf(out, "%s>\n]", vn->val->StrVal);
            break;

        case Integer:
            fprintf(out, "%lu>\n]", vn->val->IntVal);
            break;

        case Float:
            fprintf(out, "%lf>\n]", vn->val->FloatVal);
            break;

        default:
            fprintf(out, "%s>\n]", TokenNameToString(vn->val->n));
            break;
    }
}

static void walkIfStmt(IfStmtNode* ifstmt, FILE* out)
{
    fprintf(out, "\nN%p[\n label=<%s>\n]", ifstmt, TokenNameToString(ifstmt->ifTok->n));

    walkNode(ifstmt->cond, out);
    walkListNode(ifstmt->body, out, "Body");
    walkNode(ifstmt->elseBody, out);

    fprintf(out, "\nN%p -> N%p", ifstmt, ifstmt->cond.gen);
    fprintf(out, "\nN%p -> N%p", ifstmt, ifstmt->body);
    if (ifstmt->elseBody.hdr.type != NODE_EMTY)
        fprintf(out, "\nN%p -> N%p", ifstmt, ifstmt->elseBody.gen);
}

static void walkForStmt(ForStmtNode* forstmt, FILE* out)
{
    fprintf(out, "\nN%p[\n label=<%s>\n]", forstmt, TokenNameToString(forstmt->forTok->n));

    walkNode(forstmt->cond, out);
    walkListNode(forstmt->body, out, "Body");

    fprintf(out, "\nN%p -> N%p", forstmt, forstmt->cond.gen);
    fprintf(out, "\nN%p -> N%p", forstmt, forstmt->body);
}

static void walkBinOp(BinOpNode* bopn, FILE* out)
{

    fprintf(out, "\nN%p[\n label=<%s>\n]", bopn, TokenNameToString(bopn->op->n));

    walkNode(bopn->left, out);
    walkNode(bopn->right, out);

    fprintf(out, "\nN%p -> N%p", bopn, bopn->left.gen);
    fprintf(out, "\nN%p -> N%p", bopn, bopn->right.gen);
}

static void walkUnOp(UnOpNode* uopn, FILE* out)
{

    fprintf(out, "\nN%p[\n label=<%s>\n]", uopn, TokenNameToString(uopn->op->n));

    walkNode(uopn->child, out);

    if (uopn->child.hdr.type != NODE_EMTY)
        fprintf(out, "\nN%p -> N%p", uopn, uopn->child.gen);
}

static void walkNode(Node n, FILE* out)
{

    switch (n.hdr.type)
    {
        case NODE_EMTY:
            return;

        case NODE_PROGRAMM:
            return walkListNode(n.ln, out, "Programm");
        case NODE_COMPOUND_STMT:
            return walkListNode(n.ln, out, "Compound");
        case NODE_PARAMS_LIST:
            return walkListNode(n.ln, out, "Parameters");
        case NODE_ARGS_LIST:
            return walkListNode(n.ln, out, "Arguments");

        case NODE_FUNCTION_DECL:
            return walkFuncDecl(n.fdn, out);

        case NODE_FUNCTION_PARAM:
            return walkFuncParam(n.fpn, out);

        case NODE_FUNCTION_CALL:
            return walkFuncCall(n.fcn, out);

        case NODE_VAR_DECL:
            return walkVarDecl(n.vdn, out);

        case NODE_VAL:
            return walkValue(n.vn, out);

        case NODE_IF_STMT:
            return walkIfStmt(n.ifstn, out);

        case NODE_FOR_STMT:
            return walkForStmt(n.forstn, out);

            // case NODE_JUMP_STMT:
            //     walkJumpStmt(n, out);
            //     break;

        case NODE_BINOP:
            return walkBinOp(n.bopn, out);

        case NODE_UNOP:
            return walkUnOp(n.uopn, out);

        default:
            return;
    }
}

void GenGraph(FILE* out, Node n)
{
    fprintf(out, header);

    walkNode(n, out);

    fprintf(out, "\n}\n");
}
