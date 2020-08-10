#include <assert.h>
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
    TK_INT,      // int
    TK_CHAR,     // char
    TK_SIZEOF,   // sizeof
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
    ND_GVAR_DECL, // グローバル変数の定義
    ND_GVAR_REF,  // グローバル変数の参照
    ND_RETURN, // return
    ND_IF,     // if
    ND_ELSE,   // else
    ND_WHILE,  // while
    ND_FOR,    // for
    ND_BLOCK,  // ブロック({...})
    ND_FUNC,   // 関数呼び出し(foo())
    ND_DEF,    // 関数定義(foo(){})
    ND_ADDR,   // 単項&
    ND_DEREF,  // 単項*
    ND_PTR_ADD, // ポインタの加算
    ND_PTR_SUB, // ポインタの減算
} NodeKind;

typedef struct Node Node;
typedef struct NodeVector NodeVector;
typedef struct LVar LVar;
typedef struct GVar GVar;
typedef struct Func Func;
typedef struct Def Def;
typedef struct Type Type;

// 抽象構文木のノードの型
struct Node {
    NodeKind kind; // ノードの種類
    Node *lhs;     // 左辺
    Node *rhs;     // 右辺
    int val;       // kindがND_NUMの場合のみ使う
    int offset;    // kindがND_LVARの場合のみ使う
    NodeVector *vector; // kindがND_BLOCK、ND_FUNC、ND_DEFの場合のみ使う
    Func *func;    // kindがND_FUNCの場合のみ使う
    Def *def;      // kindがND_DEFの場合のみ使う
    Type *type;    // ノードの型
    GVar *gvar;    // kindがND_GVAR_DECL, ND_GVAR_REFの場合のみ使う
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

// 関数定義の型
struct Def {
    char *name;    // 関数の名前
    int len;       // 名前の長さ
    LVar *locals;  // ローカル変数
};

// 変数の型の種類
typedef enum {
    INT,
    PTR,
    ARRAY,
    CHAR,
} TypeKind;

// 変数の型を表す型
struct Type {
    TypeKind ty;
    int size;
    struct Type *ptr_to;
    size_t array_size;
};

// ローカル変数の型
struct LVar {
    LVar *next; // 次の変数かNULL
    char *name; // 変数の名前
    int len;    // 名前の長さ
    int offset; // RBPからのオフセット
    Type *type; // 変数の型
};

// ローカル変数のリスト
LVar *locals;

// グローバル変数の型
struct GVar {
    GVar *next;   // 次の変数かNULL
    char *name;   // 変数の名前
    int len;      // 名前の長さ
    Type *type;   // 変数の型
};

// グローバル変数のリスト
GVar *globals;

// パース結果
extern Node *code[100];

// 入力プログラム
extern char *user_input;

// parse.c
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
Node *declaration(void);
Node *func_decl(Type *return_type, Token *name);
Node *gvar_decl(Type *type, Token *name);
Node *lvar_decl();
Node *stmt(void);
Node *expr(void);
Node *assign(void);
Node *equality(void);
Node *relational(void);
Node *add(void);
Node *mul(void);
Node *unary(void);
Node *primary(void);

// codegen.c
void gen(Node *node);

NodeVector *new_node_vector();
void node_vector_add(NodeVector *vector, Node *node);
Node *node_vector_ref(NodeVector *vector, int index);

// type.c
extern bool is_pointer(Node *node);
extern void add_type(Node *node);
extern Type *int_type;
extern Type *char_type;
extern Type *new_type(TypeKind ty, int size, Type *ptr_to, size_t array_size);
