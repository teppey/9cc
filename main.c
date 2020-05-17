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

    for (int i = 0; code[i]; i++) {
        Node *node = code[i];
        if (node->kind != ND_DEF)
            error("関数ではありません");
        gen(node);
    }

    return 0;
}
