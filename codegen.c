#include "sobacc.h"

static int label_count;

// スタックトップの値を変数のアドレスとして
// その変数の値をスタックトップに設定する
static void load() {
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
}

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

    if (node->kind == ND_FOR) {
        int label_begin = label_count++;
        int label_end = label_count++;
        // init
        if (node->lhs)
            gen(node->lhs);
        printf(".Lbegin%d:\n", label_begin);
        // test
        if (node->rhs->lhs) {
            gen(node->rhs->lhs);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .Lend%d\n", label_end);
        }
        // body
        gen(node->rhs->rhs->rhs->lhs);
        // update
        if (node->rhs->rhs->lhs)
            gen(node->rhs->rhs->lhs);
        printf("  jmp .Lbegin%d\n", label_begin);
        printf(".Lend%d:\n", label_end);
        return;
    }

    if (node->kind == ND_BLOCK) {
        for (int i = 0; i < node->vector->count; i++) {
            gen(node_vector_ref(node->vector, i));
        }
        return;
    }

    // TODO: call命令を発行する前にRSPが16の倍数になるように調整する
    if (node->kind == ND_FUNC) {
        // 第1引数
        if (node->vector->count > 0) {
            gen(node_vector_ref(node->vector, 0));
            printf("  pop rdi\n");
        }
        // 第2引数
        if (node->vector->count > 1) {
            gen(node_vector_ref(node->vector, 1));
            printf("  pop rsi\n");
        }
        // 第3引数
        if (node->vector->count > 2) {
            gen(node_vector_ref(node->vector, 2));
            printf("  pop rdx\n");
        }
        // 第4引数
        if (node->vector->count > 3) {
            gen(node_vector_ref(node->vector, 3));
            printf("  pop rcx\n");
        }
        // 第5引数
        if (node->vector->count > 4) {
            gen(node_vector_ref(node->vector, 4));
            printf("  pop R8\n");
        }
        // 第6引数
        if (node->vector->count > 5) {
            gen(node_vector_ref(node->vector, 5));
            printf("  pop R9\n");
        }
        printf("  call %.*s\n", node->func->len, node->func->name);
        // 関数の戻り値をスタックトップに入れる
        printf("  push rax\n");
        return;
    }

    if (node->kind == ND_DEF) {
        printf(".global %.*s\n", node->def->len, node->def->name);
        printf("%.*s:\n", node->def->len, node->def->name);

        // プロローグ
        printf("  push rbp\n");
        printf("  mov rbp, rsp\n");

        // パラメーターの個数分の領域をスタックに確保
        // 最大6つまで

        // 第1引数
        if (node->vector->count > 0)
            printf("  push rdi\n");
        // 第2引数
        if (node->vector->count > 1)
            printf("  push rsi\n");
        // 第3引数
        if (node->vector->count > 2)
            printf("  push rdx\n");
        // 第4引数
        if (node->vector->count > 3)
            printf("  push rcx\n");
        // 第5引数
        if (node->vector->count > 4)
            printf("  push R8\n");
        // 第6引数
        if (node->vector->count > 5)
            printf("  push R9\n");

        // ローカル変数の領域をスタックに確保
        int params_offset = node->vector->count * 8;
        int locals_offset = 0;
        for (LVar *lvar = node->def->locals; lvar; lvar = lvar->next)
            locals_offset += lvar->type->size;
        //assert(locals_offset >= params_offset);
        if (locals_offset - params_offset > 0)
            printf("  sub rsp, %d\n", locals_offset - params_offset);

        // 関数本体
        // TODO: スタック溢れへの対応
        gen(node->lhs);

        // エピローグ
        // 最後の式の結果がRAXに残っているのでそれが返り値になる
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
        return;
    }

    switch (node->kind) {
        case ND_NUM:
            printf("  push %d\n", node->val);
            return;
        case ND_LVAR:
            gen_lval(node);
            if (node->type->ty != ARRAY)
                load();
            return;
        case ND_ASSIGN:
            // 左辺値をスタックにpush
            if (node->lhs->kind == ND_DEREF)
                gen(node->lhs->lhs);
             else
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
        case ND_ADDR:
            gen_lval(node->lhs);
            return;
        case ND_DEREF:
            gen(node->lhs);
            if (node->type->ty != ARRAY)
                load();
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
        case ND_PTR_ADD:
            if (is_pointer(node->lhs)) {
                printf("  imul rdi, %d\n", node->lhs->type->ptr_to->size);
            } else if (is_pointer(node->rhs)) {
                printf("  imul rax, %d\n", node->rhs->type->ptr_to->size);
            } else {
                assert(0);
            }
            printf("  add rax, rdi\n");
            break;
        case ND_PTR_SUB:
            if (is_pointer(node->lhs)) {
                printf("  imul rdi, %d\n", node->lhs->type->ptr_to->size);
            } else if (is_pointer(node->rhs)) {
                printf("  imul rax, %d\n", node->rhs->type->ptr_to->size);
            } else {
                assert(0);
            }
            printf("  sub rax, rdi\n");
            break;
    }

    printf("  push rax\n");
}
