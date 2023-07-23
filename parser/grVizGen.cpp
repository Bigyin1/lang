#include "parser/grVizGen.hpp"

#include "lexer/lexemes.hpp"

static void generateConnection(void* fromNodeAddr, void* toNodeAddr, FILE* out)
{
    fprintf(out, "\nN%p -> N%p", fromNodeAddr, toNodeAddr);
}

static void walkNode(Node n, FILE* out);

static void walkListNode(ListNode* ln, FILE* out, const char* name)
{

    fprintf(out, "\nN%p[\n label=<%s>\n]", ln, name);

    for (size_t i = 0; i < ln->chLen; i++)
        walkNode(ln->children[i], out);

    for (size_t i = 0; i < ln->chLen; i++)
        generateConnection(ln, ln->children[i].rawPtr, out);
}

static void walkFuncDecl(FuncDeclNode* fd, FILE* out)
{
    fprintf(out,
            "\nN%p[\nshape=record\n"
            "label=<func | %s | %s>\n]",
            fd, fd->fName->StrVal, TokenNameToString(fd->retTypeName->n));

    walkListNode(fd->params, out, "Parameters");
    walkListNode(fd->body, out, "Body");

    generateConnection(fd, fd->params, out);
    generateConnection(fd, fd->body, out);
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

    generateConnection(fcn, fcn->args, out);
}

static void walkVarDecl(VarDeclNode* vdn, FILE* out)
{

    fprintf(out,
            "\nN%p[\nshape=record\n"
            "label=<%s | %s>\n]",
            vdn, vdn->varName->StrVal, TokenNameToString(vdn->typeName->n));

    walkNode(vdn->initVal, out);

    if (vdn->initVal.hdr.type != NODE_EMPTY)
        generateConnection(vdn, vdn->initVal.rawPtr, out);
}

static void walkValue(ValNode* vn, FILE* out)
{

    fprintf(out,
            "\nN%p[\n"
            "label=<",
            vn);

    switch (vn->val->n)
    {
        case TOK_ID:
            fprintf(out, "%s>\n]", vn->val->StrVal);
            break;

        case TOK_Integer:
            fprintf(out, "%lu>\n]", vn->val->IntVal);
            break;

        case TOK_Float:
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

    generateConnection(ifstmt, ifstmt->cond.rawPtr, out);
    generateConnection(ifstmt, ifstmt->body, out);

    if (ifstmt->elseBody.hdr.type != NODE_EMPTY)
        generateConnection(ifstmt, ifstmt->elseBody.rawPtr, out);
}

static void walkForStmt(ForStmtNode* forstmt, FILE* out)
{
    fprintf(out, "\nN%p[\n label=<%s>\n]", forstmt, TokenNameToString(forstmt->forTok->n));

    walkNode(forstmt->cond, out);
    walkListNode(forstmt->body, out, "Body");

    generateConnection(forstmt, forstmt->cond.rawPtr, out);
    generateConnection(forstmt, forstmt->body, out);
}

static void walkBinOp(BinOpNode* bopn, FILE* out)
{

    fprintf(out, "\nN%p[\n label=<%s>\n]", bopn, TokenNameToString(bopn->op->n));

    walkNode(bopn->left, out);
    walkNode(bopn->right, out);

    generateConnection(bopn, bopn->left.rawPtr, out);
    generateConnection(bopn, bopn->right.rawPtr, out);
}

static void walkUnOp(UnOpNode* uopn, FILE* out)
{

    fprintf(out, "\nN%p[\n label=<%s>\n]", uopn, TokenNameToString(uopn->op->n));

    walkNode(uopn->child, out);

    if (uopn->child.hdr.type != NODE_EMPTY)
        generateConnection(uopn, uopn->child.rawPtr, out);
}

static void walkNode(Node n, FILE* out)
{

    switch (n.hdr.type)
    {
        case NODE_EMPTY:
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

        case NODE_BINOP:
            return walkBinOp(n.bopn, out);

        case NODE_UNOP:
            return walkUnOp(n.uopn, out);

        default:
            return;
    }
}

static const char* header = "digraph expr {"
                            "fontname=\"Helvetica,Arial,sans-serif\""
                            "node [fontname=\"Helvetica,Arial,sans-serif\"]"
                            "edge [fontname=\"Helvetica,Arial,sans-serif\"]"
                            "node [shape=box];";

void GenGraph(FILE* out, Node n)
{
    fprintf(out, header);

    walkNode(n, out);

    fprintf(out, "\n}\n");
}
