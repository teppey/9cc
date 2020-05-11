#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの種類
typedef enum {
    TK_RESERVED, // 記号
    TK_IDENT,    // 識別子
    TK_NUM,      // 整数トークン
    TK_EOF,      // 入力の終わりを表すトークン
    TK_RETURN,   // return
    TK_IF,       // if
    TK_ELSE,     // else
    TK_WHILE,    // while
    TK_FOR,      // for
} TokenKind;

typedef struct Token Token;

// トークン型
struct Token {
    TokenKind kind; // トークンの型
    Token *next;    // 次の入力トークン
    int val;        // kindがTK_NUMの場合、その数値
    char *str;      // トークン文字列
    int len;        // トークンの長さ
};

// 現在着目しているトークン
extern Token *token;

// 抽象構文木のノードの種類
typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_NUM, // 整数
    ND_LT,  // <
    ND_LTE, // <=
    ND_EQ,  // ==
    ND_NEQ, // !=
    ND_ASSIGN, // =
    ND_LVAR,   // ローカル変数
    ND_RETURN, // return
    ND_IF,     // if
    ND_ELSE,   // else
    ND_WHILE,  // while
    ND_FOR,    // for
    ND_BLOCK,  // ブロック({...})
    ND_FUNC,   // 関数呼び出し(foo())
} NodeKind;

typedef struct Node Node;
typedef struct NodeVector NodeVector;
typedef struct Func Func;

// 抽象構文木のノードの型
struct Node {
    NodeKind kind; // ノードの型
    Node *lhs;     // 左辺
    Node *rhs;     // 右辺
    int val;       // kindがND_NUMの場合のみ使う
    int offset;    // kindがND_LVARの場合のみ使う
    NodeVector *vector; // kindがND_BLOCKの場合のみ使う
    Func *func;    // kindがND_FUNCの場合のみ使う
};

// ノードベクタの型
struct NodeVector {
    Node **data;
    int size;
    int count;
};

// 関数呼び出しの型
struct Func {
    char *name; // 関数の名前
    int len;    // 名前の長さ
};

typedef struct LVar LVar;

// ローカル変数の型
struct LVar {
    LVar *next; // 次の変数かNULL
    char *name; // 変数の名前
    int len;    // 名前の長さ
    int offset; // RBPからのオフセット
};

// ローカル変数
LVar *locals;

// パース結果
extern Node *code[100];

// 入力プログラム
extern char *user_input;

void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);
bool consume(char *op);
void expect(char *op);
int expect_number();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
int is_alnum(char c);
void tokenize(void);
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
void program(void);
Node *stmt(void);
Node *expr(void);
Node *assign(void);
Node *equality(void);
Node *relational(void);
Node *add(void);
Node *mul(void);
Node *unary(void);
Node *primary(void);
void gen(Node *node);

NodeVector *new_node_vector();
void node_vector_add(NodeVector *vector, Node *node);
Node *node_vector_ref(NodeVector *vector, int index);
