#include "parse_stmt.hpp"

#include "nodes.hpp"
#include "parse_expr.hpp"
#include "parser.hpp"

static Node varDecl(Parser* p)
{
    if (p->HasErr)
        return Node{};

    eatToken(p, VAR);

    Token* varName = getCurrToken(p);
    eatToken(p, ID);

    Token* varType = getCurrToken(p);
    eatToken(p, FloatType | IntType | BoolType);

    Node init = {};

    if (currTokenHasName(p, ASSIGN))
    {
        eatToken(p, ASSIGN);
        init = expr(p);
    }

    return NewVarDeclNode(varName, varType, init);
}

Node compoundStmt(Parser* p)
{
    if (p->HasErr)
        return Node{};

    eatToken(p, LBRACE);

    Node ln = NewListNode(NODE_COMPOUND_STMT);

    while (!currTokenHasName(p, RBRACE))
    {
        if (currTokenHasName(p, VAR))
            ListNodeAddChild(ln.ln, varDecl(p));
        else
            ListNodeAddChild(ln.ln, stmt(p));

        if (p->HasErr)
            return ln;
    }

    eatToken(p, RBRACE);

    return ln;
}

static Node assignStmt(Parser* p)
{
    if (p->HasErr)
        return Node{};

    Node var = NewValNode(getCurrToken(p));
    eatToken(p, ID);

    Token* assign = getCurrToken(p);
    eatToken(p, ASSIGN);

    Node val = expr(p);

    return NewBinOpNode(var, val, assign);
}

static Node selectionStmt(Parser* p)
{
    if (p->HasErr)
        return Node{};

    Token* ifTok = getCurrToken(p);
    eatToken(p, IF);

    Node cond = expr(p);

    ListNode* body = compoundStmt(p).ln;

    Node elseBody = {};
    if (currTokenHasName(p, ELSE))
    {
        eatToken(p, ELSE);
        if (currTokenHasName(p, IF))
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
    eatToken(p, FOR);

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

    if (op->n == RETURN)
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

    if (currTokenHasName(p, LBRACE))
        return compoundStmt(p);

    if (currTokenHasName(p, ID))
    {
        TokenName next = peek(p);
        if (next == ASSIGN)
            return assignStmt(p);

        if (next == LPAREN)
            return funccall(p);
    }

    if (currTokenHasName(p, IF))
        return selectionStmt(p);

    if (currTokenHasName(p, FOR))
        return iterationStmt(p);

    if (currTokenHasName(p, BREAK | CONTINUE | RETURN))
        return jumpStmt(p);

    emitError(p, LBRACE | ID | IF | FOR | BREAK | CONTINUE | RETURN);

    return Node{};
}
