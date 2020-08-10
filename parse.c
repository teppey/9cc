#include "sobacc.h"

// 現在着目しているトークン
Token *token;

// パース結果
Node *code[100];

// 入力プログラム
char *user_input;

// エラー箇所を報告する
void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, ""); // pos個の空白を出力
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて真を返す。
// それ以外の場合には偽を返す。
bool consume(char *op) {
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        return false;
    token = token->next;
    return true;
}

// 次のトークンがreturnのときには、トークンを1つ読み進めて真を返す。
// それ以外の場合には偽を返す。
bool consume_return() {
    if (token->kind != TK_RETURN)
        return false;
    token = token->next;
    return true;
}

// 次のトークンがifのときには、トークンを1つ読み進めて真を返す。
// それ以外の場合には偽を返す。
bool consume_if() {
    if (token->kind != TK_IF)
        return false;
    token = token->next;
    return true;
}

// 次のトークンがelseのときには、トークンを1つ読み進めて真を返す。
// それ以外の場合には偽を返す。
bool consume_else() {
    if (token->kind != TK_ELSE)
        return false;
    token = token->next;
    return true;
}

// 次のトークンがwhileのときには、トークンを1つ読み進めて真を返す。
// それ以外の場合には偽を返す。
bool consume_while() {
    if (token->kind != TK_WHILE)
        return false;
    token = token->next;
    return true;
}

// 次のトークンがforのときには、トークンを1つ読み進めて真を返す。
// それ以外の場合には偽を返す。
bool consume_for() {
    if (token->kind != TK_FOR)
        return false;
    token = token->next;
    return true;
}

// 次のトークンがintのときには、トークンを1つ読み進めて真を返す。
// それ以外の場合には偽を返す。
bool consume_int() {
    if (token->kind != TK_INT)
        return false;
    token = token->next;
    return true;
}

// 次のトークンがcharのときには、トークンを1つ読み進めて真を返す。
// それ以外の場合には偽を返す。
bool consume_char() {
    if (token->kind != TK_CHAR)
        return false;
    token = token->next;
    return true;
}

// 次のトークンがsizeofのときには、トークンを1つ読み進めて真を返す。
// それ以外の場合には偽を返す。
bool consume_sizeof() {
    if (token->kind != TK_SIZEOF)
        return false;
    token = token->next;
    return true;
}

// 次のトークンが識別子のときには、そのトークンを返しトークンを1つ読み進める。
// それ以外の場合にはNULLを返す。
Token *consume_ident() {
    Token *tok = token;
    if (tok->kind != TK_IDENT)
        return NULL;
    token = tok->next;
    return tok;
}


// 次のトークンが期待している記号のときには、トークンを1つ読み進める。それ以
// 外の場合にはエラーを報告する。
void expect(char *op) {
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        error_at(token->str, "\"%s\"ではありません", op);
    token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。それ以外
// の場合にはエラーを報告する。
int expect_number() {
    if (token->kind != TK_NUM)
        error_at(token->str, "数ではありません");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

int is_alnum(char c) {
    return ('a' <= c && c <= 'z') ||
           ('A' <= c && c <= 'Z') ||
           ('0' <= c && c <= '9') ||
           (c == '_');
}

// 入力文字列user_inputをトークナイズして
// tokenをトークンのリストの先頭にセットする
void tokenize() {
    Token head;
    head.next = NULL;
    Token *cur = &head;
    char *p = user_input;

    while (*p) {
        // 空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        // 長いトークンから先にトークナイズする
        if (strncmp(p, "<=", 2) == 0 ||
            strncmp(p, ">=", 2) == 0 ||
            strncmp(p, "==", 2) == 0 ||
            strncmp(p, "!=", 2) == 0) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        // return
        if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
            cur = new_token(TK_RETURN, cur, p, 6);
            p += 6;
            continue;
        }

        // if
        if (strncmp(p, "if", 2) == 0 && !is_alnum(p[2])) {
            //cur = new_token(TK_IF, cur, "if", 2);
            cur = new_token(TK_IF, cur, p, 2);
            p += 2;
            continue;
        }

        // else
        if (strncmp(p, "else", 4) == 0 && !is_alnum(p[4])) {
            cur = new_token(TK_ELSE, cur, p, 4);
            p += 4;
            continue;
        }

        // while
        if (strncmp(p, "while", 5) == 0 && !is_alnum(p[5])) {
            cur = new_token(TK_WHILE, cur, p, 5);
            p += 5;
            continue;
        }

        // for
        if (strncmp(p, "for", 3) == 0 && !is_alnum(p[3])) {
            cur = new_token(TK_FOR, cur, p, 3);
            p += 3;
            continue;
        }

        // int
        if (strncmp(p, "int", 3) == 0 && !is_alnum(p[3])) {
            cur = new_token(TK_INT, cur, p, 3);
            p += 3;
            continue;
        }

        // char
        if (strncmp(p, "char", 4) == 0 && !is_alnum(p[4])) {
            cur = new_token(TK_CHAR, cur, p, 4);
            p += 4;
            continue;
        }

        // sizeof
        if (strncmp(p, "sizeof", 6) == 0 && !is_alnum(p[6])) {
            cur = new_token(TK_SIZEOF, cur, p, 6);
            p += 6;
            continue;
        }

        if ('a' <= *p && *p <= 'z') {
            char *q = p + 1;
            while (is_alnum(*q))
                q++;
            int len = q - p;
            cur = new_token(TK_IDENT, cur, p, len);
            p += len;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' ||
            *p == '<' || *p == '>' || *p == '=' || *p == ';' || *p == '{' || *p == '}' ||
            *p == ',' || *p == '&' || *p == '[' || *p == ']') {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if (isdigit(*p)) {
            // 数値の場合、トークンの長さとしてとりあえず0を指定しておく
            cur = new_token(TK_NUM, cur, p, 0);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error_at(p, "トークナイズできません");
    }

    new_token(TK_EOF, cur, p, 0);
    token = head.next;
}

// ローカル変数を名前で検索する。見つからなかった場合はNULLを返す。
LVar *find_lvar(Token *tok) {
    for (LVar *var = locals; var; var = var->next)
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
            return var;
    return NULL;
}

// グローバル変数を名前で検索する。見つからなかった場合はNULLを返す。
GVar *find_gvar(Token *tok) {
    for (GVar *var = globals; var; var = var->next)
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
            return var;
    return NULL;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

void program() {
    int i = 0;
    while (!at_eof())
        code[i++] = declaration();
    code[i] = NULL;
}

Node *declaration() {
    Type *type = NULL;
    if (consume_int())
        type = int_type;
    else if (consume_char())
        type = char_type;
    else
        error_at(token->str, "intまたはcharではありません");

    assert(type != NULL);

    // ポインタ
    while (consume("*")) {
        Type *ptr = new_type(PTR, 8, type, 0);
        type = ptr;
    }

    // 識別子が来ているはずなのでそれを読む
    Token *tok = consume_ident();
    if (!tok)
        error_at(token->str, "識別子ではありません");

    // トークンを1つ先読みし"(" なら関数定義、そうでなければグローバル変数定義
    if (token->kind == TK_RESERVED && memcmp(token->str, "(", 1) == 0)
        return func_decl(type, tok);
    else
        return gvar_decl(type, tok);
}

// 関数定義のパース
// 型と識別子までのパースは終わって引数でそれらが渡される前提
// TODO: 戻り値の型を保持する
Node *func_decl(Type *return_type, Token *name) {
    Def *def = calloc(1, sizeof(Def));
    def->name = name->str;
    def->len = name->len;
    def->locals = NULL;

    // パラメーターリスト
    // 各パラメーターはローカル変数としてパース
    expect("(");
    NodeVector *params = new_node_vector();
    while (!consume(")")) {
        if (!consume_int())
            error_at(token->str, "intではありません");

        Type *type = int_type;
        while (consume("*")) {
            Type *ptr = new_type(PTR, 8, type, 0);
            type = ptr;
        }

        Token *tok = consume_ident();
        if (!tok)
            error_at(token->str, "関数の引数が識別子ではありません");

        LVar *lvar = calloc(1, sizeof(LVar));
        lvar->next = def->locals;
        lvar->name = tok->str;
        lvar->len = tok->len;
        if (def->locals)
            lvar->offset = def->locals->offset + type->size;
        else
            lvar->offset = type->size;
        lvar->type = type;
        def->locals = lvar;
        Node *param = new_node(ND_LVAR, NULL, NULL);
        param->offset = lvar->offset;
        param->type = lvar->type;
        node_vector_add(params, param);
        consume(",");
    }

    // グローバル変数であるローカル変数リストの値を一旦保存して
    // 現在の関数のパラメータリストを設定
    LVar *locals_save = locals;
    locals = def->locals;

    // 関数本体をパース
    expect("{");
    NodeVector *stmts = new_node_vector();
    while (!consume("}"))
        node_vector_add(stmts, stmt());
    Node *body = new_node(ND_BLOCK, NULL, NULL);
    body->vector = stmts;

    // 関数定義のローカル変数リストを更新して
    // グローバル変数を元に戻す
    def->locals = locals;
    locals = locals_save;

    Node *node = new_node(ND_DEF, body, NULL);
    node->def = def;
    node->vector = params;
    return node;
}

Node *stmt() {
    Node *node;
    Node *test;
    Node *body;

    if (consume("{")) {
        NodeVector *vector = new_node_vector();
        while (!consume("}"))
            node_vector_add(vector, stmt());
        node = new_node(ND_BLOCK, NULL, NULL);
        node->vector = vector;
        return node;
    }

    if (consume_if()) {
        expect("(");
        test = expr();
        expect(")");
        body = stmt();
        if (consume_else()) {
            node = new_node(ND_IF, test, new_node(ND_ELSE, body, stmt()));
        } else {
            node = new_node(ND_IF, test, body);
        }
        return node;
    }

    if (consume_while()) {
        expect("(");
        test = expr();
        expect(")");
        body = stmt();
        node = new_node(ND_WHILE, test, body);
        return node;
    }

    if (consume_for()) {
        Node *init = NULL;
        Node *test = NULL;
        Node *update = NULL;
        expect("(");
        if (!consume(";")) {
            init = expr();
            expect(";");
        }
        if (!consume(";")) {
            test = expr();
            expect(";");
        }
        if (!consume(")")) {
            update = expr();
            expect(")");
        }
        node = new_node(ND_FOR, init,
                new_node(ND_FOR, test,
                    new_node(ND_FOR, update,
                        new_node(ND_FOR, stmt(), NULL))));
        return node;
    }

    if (consume_return()) {
        node = new_node(ND_RETURN, expr(), NULL);
    } else if (token->kind == TK_INT || token->kind == TK_CHAR) {
        node = lvar_decl();
    } else {
        node = expr();
    }

    expect(";");
    return node;
}

// グローバル変数定義をパース
// 型と識別子までのパースは終わって引数でそれらが渡される前提
// TODO: gvar_declとlvar_declをvar_declとしてまとめる(型の前半部分をパースする関数を用意する)
Node *gvar_decl(Type *type, Token *name) {
    // TODO: 変数名の重複への対応

    // TODO: 入れ子の配列
    if (consume("[")) {
        int array_size =  expect_number();
        expect("]");
        Type *array = new_type(ARRAY, type->size * array_size, type, array_size);
        type = array;
    }
    expect(";");

    GVar *gvar = calloc(1, sizeof(GVar));
    gvar->next = globals;
    gvar->name = name->str;
    gvar->len = name->len;
    gvar->type = type;
    globals = gvar;

    Node *node = new_node(ND_GVAR_DECL, NULL, NULL);
    node->type = gvar->type;
    node->gvar = gvar;
    return node;
}

// ローカル変数定義をパース
Node *lvar_decl() {
    Type *type = NULL;
    if (consume_int())
        type = int_type;
    else if (consume_char())
        type = char_type;
    else
        error_at(token->str, "intまたはcharではありません");

    assert(type != NULL);

    // ポインタ
    while (consume("*")) {
        Type *ptr = new_type(PTR, 8, type, 0);
        type = ptr;
    }

    Token *tok = consume_ident();
    if (!tok)
        error_at(token->str, "変数名ではありません");

    LVar *lvar = find_lvar(tok);
    if (lvar)
        error_at(token->str, "変数名が重複しています");

    // TODO: 入れ子の配列
    if (consume("[")) {
        int array_size =  expect_number();
        expect("]");
        Type *array = new_type(ARRAY, type->size * array_size, type, array_size);
        type = array;
    }

    lvar = calloc(1, sizeof(LVar));
    lvar->next = locals;
    lvar->name = tok->str;
    lvar->len = tok->len;
    lvar->offset = (locals ? locals->offset : 0) + type->size;
    lvar->type = type;
    Node *node = new_node(ND_LVAR, NULL, NULL);
    node->offset = lvar->offset;
    node->type = lvar->type;
    locals = lvar;
    return node;
}

Node *expr() {
    return assign();
}

Node *assign() {
    Node *node = equality();
    if (consume("="))
        node = new_node(ND_ASSIGN, node, assign());
    return node;
}

Node *equality() {
    Node *node = relational();

    for (;;) {
        if (consume("=="))
            node = new_node(ND_EQ, node, relational());
        else if (consume("!="))
            node = new_node(ND_NEQ, node, relational());
        else
            return node;
    }
}

Node *relational() {
    Node *node = add();

    for (;;) {
        if (consume("<"))
            node = new_node(ND_LT, node, add());
        else if (consume("<="))
            node = new_node(ND_LTE, node, add());
        else if (consume(">"))
            node = new_node(ND_LT, add(), node);
        else if (consume(">="))
            node = new_node(ND_LTE, add(), node);
        else
            return node;
    }
}

Node *add() {
    Node *lhs = mul();
    Node *rhs;

    add_type(lhs);

    for (;;) {
        if (consume("+")) {
            rhs = mul();
            add_type(rhs);
            if (is_pointer(lhs) || is_pointer(rhs)) {
                lhs = new_node(ND_PTR_ADD, lhs, rhs);
            } else {
                lhs = new_node(ND_ADD, lhs, rhs);
            }
            add_type(lhs);
        } else if (consume("-")) {
            rhs = mul();
            add_type(rhs);
            if (is_pointer(lhs) || is_pointer(rhs)) {
                lhs = new_node(ND_PTR_SUB, lhs, rhs);
            } else {
                lhs = new_node(ND_SUB, lhs, rhs);
            }
            add_type(lhs);
        } else {
            return lhs;
        }
    }
}

Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume("*"))
            node = new_node(ND_MUL, node, unary());
        else if (consume("/"))
            node = new_node(ND_DIV, node, unary());
        else
            return node;
    }
}

Node *unary() {
    if (consume_sizeof()) {
        // TODO: 配列への対応
        // TODO: node->type->sizeを返す
        Node *node = unary();
        add_type(node);
        assert(node->type);
        if (node->type->ty == INT)
            return new_node_num(4);
        else if (node->type->ty == CHAR)
            return new_node_num(1);
        else if (node->type->ty == PTR)
            return new_node_num(8);
        else
            return new_node_num(node->type->size);
    }

    if (consume("*"))
        return new_node(ND_DEREF, unary(), NULL);
    else if (consume("&"))
        return new_node(ND_ADDR, unary(), NULL);
    else if (consume("+"))
        return primary();
    else if (consume("-"))
        return new_node(ND_SUB, new_node_num(0), primary());
    return primary();
}

Node *primary() {
    // 次のトークンが"("なら、"(" expr ")"のはず
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    // 次のトークンが識別子なら次のいずれか:
    //   - 関数呼び出し
    //   - 配列への添字でのアクセス
    //   - ローカル変数を表すノードを返す
    Token *tok = consume_ident();
    if (tok) {
        if (consume("(")) {
            // 6つまでの引数をサポート
            NodeVector *vector = new_node_vector();
            for (int i = 0; !consume(")"); i++) {
                if (i > 5)
                    error("too many arguments: %s", tok->str);
                node_vector_add(vector, expr());
                consume(",");
            }
            Func *func = calloc(1, sizeof(Func));
            func->name = tok->str;
            func->len = tok->len;
            Node *node = new_node(ND_FUNC, NULL, NULL);
            node->func = func;
            node->vector = vector;
            return node;
        }

        // 配列の添字: x[y]は*(x+y)と等価
        if (consume("[")) {
            Node *rhs = expr();
            expect("]");

            // ローカル変数を探す
            LVar *lvar = find_lvar(tok);
            if (lvar) {
                // 例: a[3]
                Node *lhs = calloc(1, sizeof(Node));
                lhs->kind = ND_LVAR;
                lhs->offset = lvar->offset;
                lhs->type = lvar->type;
                return new_node(ND_DEREF, new_node(ND_PTR_ADD, lhs, rhs), NULL);
            }

            // ローカル変数が見つからなければグローバル変数を探す
            GVar *gvar = find_gvar(tok);
            if (gvar) {
                // 例: a[3]
                Node *lhs = calloc(1, sizeof(Node));
                lhs->kind = ND_GVAR_REF;
                lhs->gvar = gvar;
                lhs->type = gvar->type;
                return new_node(ND_DEREF, new_node(ND_PTR_ADD, lhs, rhs), NULL);
            }

            error_at(token->str, "未定義の変数です");
        }

        Node *node = calloc(1, sizeof(Node));

        // ローカル変数を探す
        LVar *lvar = find_lvar(tok);
        if (lvar) {
            node->kind = ND_LVAR;
            node->offset = lvar->offset;
            node->type = lvar->type;
            return node;
        }

        // ローカル変数が見つからなければグローバル変数を探す
        GVar *gvar = find_gvar(tok);
        if (gvar) {
            node->kind = ND_GVAR_REF;
            node->gvar = gvar;
            node->type = gvar->type;
            return node;
        }

        error_at(token->str, "未定義の変数です");
    }

    int num = expect_number();
    if (consume("[")) {
        // 例: 3[a]
        Node *rhs = expr();
        expect("]");
        Node *lhs = new_node_num(num);
        return new_node(ND_DEREF, new_node(ND_PTR_ADD, lhs, rhs), NULL);
    }
    return new_node_num(num);
}
