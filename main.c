#include "sobacc.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        error("引数の個数が正しくありません");
        return 1;
    }

    // トークナイズしてパースする
    // 結果はcodeに保存される
    user_input = argv[1];
    tokenize();
    program();

    printf(".intel_syntax noprefix\n");

    // グローバル変数定義のコードをデータセクションに出力
    printf(".data\n");
    for (int i = 0; code[i]; i++) {
        Node *node = code[i];
        assert(node->kind == ND_DEF || node->kind == ND_GVAR_DECL);
        if (node->kind == ND_GVAR_DECL)
            gen(node);
    }

    // 関数定義をテキストセクションに出力
    printf(".text\n");
    for (int i = 0; code[i]; i++) {
        Node *node = code[i];
        assert(node->kind == ND_DEF || node->kind == ND_GVAR_DECL);
        if (node->kind == ND_DEF)
            gen(node);
    }

    return 0;
}
