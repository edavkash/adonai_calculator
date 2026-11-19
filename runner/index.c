#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

#define BUFSIZE 1024

int sp = 0;
double val[BUFSIZE];
char *current_input;

typedef enum
{
    TK_LP,
    TK_NUM,
    TK_PUNCT,
    TK_RP,
    TK_EOF
} tokentype;

typedef enum
{
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM
} nodetype;

typedef struct Token Token;
typedef struct Node Node;

struct Node
{
    int val;
    nodetype type;
    Node *lhs, *rhs;
    Node *next;
};

struct Token
{
    int len;
    int val;
    char *loc;
    tokentype type;
    Token *next;
};


void error(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(1);
}

void emit(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf( fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
}

void verror_at(char *location, char *fmt, va_list ap)
{
    int position = location - current_input;
    fprintf(stderr, "%s\n", current_input);
    fprintf(stderr, "%*s", position, ""); // print position spaces.
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error_at(char *location, char *fmt,...)
{
    va_list ap;
    va_start(ap,fmt);
    verror_at(location,fmt,ap);
}

void error_tok(Token *tok, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    verror_at(tok->loc, fmt, ap);
}

// Consumes the current token if it matches `op`.
bool equal(Token *tok, char *op)
{
    return memcmp(tok->loc, op, tok->len) == 0 && op[tok->len] == '\0';
}

// Ensure that the current token is `op`.
Token *skip(Token *tok, char *op)
{
    if (!equal(tok, op))
        error_tok(tok,"expected '%s'", op);
    return tok->next;
}

int get_number(Token *tok)
{
    if (tok->type != TK_NUM)
        emit("expected a number");

    return tok->val;
}

Token *new_token(tokentype type, char *start, char *end)
{
    Token *tok = calloc(1, sizeof(Token));
    tok->type = type;
    tok->loc = start;
    tok->len = end - start;
    return tok;
}

Token *tokenize()
{
    char *p = current_input;
    Token head = {};
    Token *cur = &head;

    while (*p)
        {
            if (isspace(*p))
                {
                    p++;
                    continue;
                }

            if (isdigit(*p))
                {
                    cur = cur->next = new_token(TK_NUM, p, p);
                    char *q = p;
                    cur->val = strtoul(p, &p, 10);
                    cur->len = p - q;
                    continue;
                }
            if (*p == '(')
                {
                    cur = cur->next = new_token(TK_LP, p, p + 1);
                    // p += cur->len;
                    p++;
                    continue;
                }

            if (*p == ')')
                {
                    cur = cur->next = new_token(TK_RP, p, p + 1);
                    // p += cur->len;
                    p++;
                    continue;
                }

            if (ispunct(*p))
                {
                    cur = cur->next = new_token(TK_PUNCT, p, p + 1);
                    // p += cur->len;
                    p++;
                    continue;
                }


            error("Unkown token type");
        }
    cur = cur->next = new_token(TK_EOF, p, p);
    return head.next;
}


Node *new_node(nodetype type)
{
    Node *node = calloc(1, sizeof(Node));
    node->type = type;
    return node;
}

Node *new_binary(nodetype type, Node *lhs, Node *rhs)
{
    Node *node = new_node(type);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_num(int val)
{
    Node *node = new_node(ND_NUM);
    node->val = val;
    return node;
}

Node *expr(Token **rest, Token *tok);
Node *mul(Token **rest, Token *tok);
Node *primary(Token **rest, Token *tok);

Node *expr(Token **rest, Token *tok)
{
    Node *node = mul(&tok, tok);

    for (;;)
        {
            if (equal(tok, "+"))
                {
                    node = new_binary(ND_ADD, node, mul(&tok, tok->next));
                    continue;
                }
            if (equal(tok, "-"))
                {
                    node = new_binary(ND_SUB, node, mul(&tok, tok->next));
                    continue;
                }
            *rest = tok;
            return node;
        }
}

Node *mul(Token **rest, Token *tok)
{
    Node *node = primary(&tok, tok);

    for (;;)
        {
            if (equal(tok, "*"))
                {
                    node = new_binary(ND_MUL, node, primary(&tok, tok->next));
                    continue;
                }
            if (equal(tok, "/"))
                {
                    node = new_binary(ND_DIV, node, primary(&tok, tok->next));
                    continue;
                }

            *rest = tok;
            return node;
        }
}

Node *primary(Token **rest, Token *tok)
{
    if (equal(tok, "("))
        {
            Node *node = expr(&tok, tok->next);
            *rest = skip(tok, ")");
            return node;
        }

    if (tok->type == TK_NUM)
        {
            Node *node = new_num(tok->val);
            *rest = tok->next;
            return node;
        }
    error_tok(tok,"expected an expression\n");
}


void codegen(Node *node)
{
    switch (node->type)
        {
        case ND_ADD:
            codegen(node->lhs);
            codegen(node->rhs);
            node->val = node->lhs->val + node->rhs->val;
            return;

        case ND_SUB:
            codegen(node->lhs);
            codegen(node->rhs);
            node->val = node->lhs->val - node->rhs->val;
            return;

        case ND_MUL:
            codegen(node->lhs);
            codegen(node->rhs);
            node->val = node->lhs->val * node->rhs->val;
            return;

        case ND_DIV:
            codegen(node->lhs);
            codegen(node->rhs);

            if (node->rhs->val == 0)
                error("division by zero");

            node->val = node->lhs->val / node->rhs->val;
            return;

        case ND_NUM:
            return;
        }
}


int main(int argc, char **argv)
{
    if (argc != 2)
        error("%s: few arguments", argv[0]);

    current_input = argv[1];
    Token *tok = tokenize();
    Node *node = expr(&tok, tok);

    if (tok->type != TK_EOF)
        error_tok(tok, "extra token");

    codegen(node);
    emit("%d\n",node->val);
    return 0;
}
