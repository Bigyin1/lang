#include "parse_stmt.hpp"

#include "nodes.hpp"
#include "parse_expr.hpp"
#include "parser.hpp"

static Node varDecl(Parser* p)
{
    if (p->HasErr)
        return Node{};

    eatToken(p, TOK_VAR);

    Token* varName = getCurrToken(p);
    eatToken(p, TOK_ID);

    Token* varType = getCurrToken(p);
    eatToken(p, TOK_FloatType | TOK_IntType | TOK_BoolType);

    Node init = {};

    if (currTokenHasName(p, TOK_ASSIGN))
    {
        eatToken(p, TOK_ASSIGN);
        init = expr(p);
    }

    return NewVarDeclNode(varName, varType, init);
}

Node compoundStmt(Parser* p)
{
    if (p->HasErr)
        return Node{};

    eatToken(p, TOK_LBRACE);

    Node ln = NewListNode(NODE_COMPOUND_STMT);

    while (!currTokenHasName(p, TOK_RBRACE))
    {
        if (currTokenHasName(p, TOK_VAR))
            ListNodeAddChild(ln.ln, varDecl(p));
        else
            ListNodeAddChild(ln.ln, stmt(p));

        if (p->HasErr)
            return ln;
    }

    eatToken(p, TOK_RBRACE);

    return ln;
}

static Node assignStmt(Parser* p)
{
    if (p->HasErr)
        return Node{};

    Node var = NewValNode(getCurrToken(p));
    eatToken(p, TOK_ID);

    Token* assign = getCurrToken(p);
    eatToken(p, TOK_ASSIGN);

    Node val = expr(p);

    return NewBinOpNode(var, val, assign);
}

static Node selectionStmt(Parser* p)
{
    if (p->HasErr)
        return Node{};

    Token* ifTok = getCurrToken(p);
    eatToken(p, TOK_IF);

    Node cond = expr(p);

    ListNode* body = compoundStmt(p).ln;

    Node elseBody = {};
    if (currTokenHasName(p, TOK_ELSE))
    {
        eatToken(p, TOK_ELSE);
        if (currTokenHasName(p, TOK_IF))
            elseBody = selectionStmt(p);
        else
            elseBody = compoundStmt(p);
    }

    return NewIfStmtNode(ifTok, cond, body, elseBody);
}

static Node iterationStmt(Parser* p)
{
    if (p->HasErr)
        return Node{};

    Token* forTok = getCurrToken(p);
    eatToken(p, TOK_FOR);

    Node cond = expr(p);

    ListNode* body = compoundStmt(p).ln;

    return NewForStmtNode(forTok, cond, body);
}

static Node jumpStmt(Parser* p)
{
    if (p->HasErr)
        return Node{};

    Token* op     = getCurrToken(p);
    Node   retVal = {};

    eatToken(p, op->n);

    if (op->n == TOK_RETURN)
    {
        size_t currTokIdx = p->currTokenIdx;

        retVal = expr(p);
        if (currTokIdx == p->currTokenIdx)
        {
            p->HasErr = false;
            p->Err    = UnexpTokenError{};
        }
    }

    return NewUnOpNode(retVal, op);
}

Node stmt(Parser* p)
{
    if (p->HasErr)
        return Node{};

    if (currTokenHasName(p, TOK_LBRACE))
        return compoundStmt(p);

    if (currTokenHasName(p, TOK_ID))
    {
        TokenName next = peek(p);
        if (next == TOK_ASSIGN)
            return assignStmt(p);

        if (next == TOK_LPAREN)
            return funccall(p);
    }

    if (currTokenHasName(p, TOK_IF))
        return selectionStmt(p);

    if (currTokenHasName(p, TOK_FOR))
        return iterationStmt(p);

    if (currTokenHasName(p, TOK_BREAK | TOK_CONTINUE | TOK_RETURN))
        return jumpStmt(p);

    emitError(p, TOK_LBRACE | TOK_ID | TOK_IF | TOK_FOR | TOK_BREAK | TOK_CONTINUE | TOK_RETURN);

    return Node{};
}
