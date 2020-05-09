#include "sobacc.h"

static int label_count;

void gen_lval(Node *node) {
    if (node->kind != ND_LVAR)
        error("代入の左辺値が変数ではありません");

    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
}

void gen(Node *node) {
    if (node->kind == ND_RETURN) {
        // returnの返り値になっている式のコードを出力
        // 出力したコードはスタックトップに1つの値を残すはず
        gen(node->lhs);
        // スタックトップの値をRAXにセット
        printf("  pop rax\n");
        // スタックポインタをベースポインタに設定
        printf("  mov rsp, rbp\n");
        // RBPレジスタに現在の関数を呼び出す前のベースポインタをセット
        printf("  pop rbp\n");
        // リターンアドレスをスタックからポップしてジャンプ
        printf("  ret\n");
        return;
    }

    if (node->kind == ND_IF) {
        int label_else;
        int label_end;
        if (node->rhs->kind == ND_ELSE) {
            // elseがある場合
            label_else = label_count++;
            label_end = label_count++;
            gen(node->lhs);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je  .Lelse%d\n", label_else);
            gen(node->rhs->lhs);
            printf("  jmp .Lend%d\n", label_end);
            printf(".Lelse%d:\n", label_else);
            gen(node->rhs->rhs);
            printf(".Lend%d:\n", label_end);
            return;
        } else {
            // elseがない場合
            label_end = label_count++;
            gen(node->lhs);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je  .Lend%d\n", label_end);
            gen(node->rhs);
            printf(".Lend%d:\n", label_end);
            return;
        }
    }

    if (node->kind == ND_WHILE) {
        int label_begin = label_count++;
        int label_end = label_count++;
        printf(".Lbegin%d:\n", label_begin);
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je  .Lend%d\n", label_end);
        gen(node->rhs);
        printf("  jmp .Lbegin%d\n", label_begin);
        printf(".Lend%d:\n", label_end);
        return;
    }

    switch (node->kind) {
        case ND_NUM:
            printf("  push %d\n", node->val);
            return;
        case ND_LVAR:
            gen_lval(node);
            // 変数のアドレスをRAXに入れる
            printf("  pop rax\n");
            // 変数の値をRAXにロード
            printf("  mov rax, [rax]\n");
            // 変数の値をスタックトップに入れる
            printf("  push rax\n");
            return;
        case ND_ASSIGN:
            // 左辺値をスタックにpush
            gen_lval(node->lhs);
            // 右辺値をスタックにpush
            gen(node->rhs);

            // 右辺値をRDIにセット
            printf("  pop rdi\n");
            // 左辺値(変数のアドレス)をRAXにセット
            printf("  pop rax\n");
            // RAXが示す変数のアドレスに右辺値をストア
            printf("  mov [rax], rdi\n");
            // ストアした値(右辺値)をスタックにpush
            printf("  push rdi\n");
            return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind) {
        case ND_ADD:
            printf("  add rax, rdi\n");
            break;
        case ND_SUB:
            printf("  sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("  imul rax, rdi\n");
            break;
        case ND_DIV:
            printf("  cqo\n");
            printf("  idiv rdi\n");
            break;
        case ND_LT:
            printf("  cmp rax, rdi\n");
            printf("  setl al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_LTE:
            printf("  cmp rax, rdi\n");
            printf("  setle al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_EQ:
            printf("  cmp rax, rdi\n");
            printf("  sete al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_NEQ:
            printf("  cmp rax, rdi\n");
            printf("  setne al\n");
            printf("  movzb rax, al\n");
            break;
    }

    printf("  push rax\n");
}
