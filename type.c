#include "sobacc.h"

Type *int_type = &(Type){ INT, 8, NULL, 0 };

Type *pointer_to(Type *base) {
    return new_type(PTR, 8, base, 0);
}

bool is_pointer(Node *node) {
    return (node->kind == ND_LVAR && node->type->ty == PTR) ||
           (node->kind == ND_LVAR && node->type->ty == ARRAY) ||
            node->kind == ND_PTR_ADD ||
            node->kind == ND_PTR_SUB;
}

void add_type(Node *node) {
    if (!node || node->type)
        return;

    add_type(node->lhs);
    add_type(node->rhs);

    if (node->kind == ND_BLOCK || node->kind == ND_FUNC || node->kind == ND_DEF) {
        for (int i = 0; i < node->vector->count; i++) {
            add_type(node_vector_ref(node->vector, i));
        }
    }

    switch (node->kind) {
        case ND_ADD:
        case ND_SUB:
        case ND_MUL:
        case ND_DIV:
        case ND_NUM:
        case ND_LT:
        case ND_LTE:
        case ND_EQ:
        case ND_NEQ:
        case ND_FUNC:
        case ND_DEF:
            node->type = int_type;
            return;
        case ND_ASSIGN:
            node->type = node->lhs->type;
            return;
        case ND_LVAR:
            // ローカル変数はtypeが設定されているはず
            assert(node->type);
            return;
        case ND_ADDR:
            if (node->lhs->type->ty == ARRAY)
                node->type = pointer_to(node->lhs->type->ptr_to);
            else
                node->type = pointer_to(node->lhs->type);
            return;
        case ND_DEREF:
            if (node->lhs->type->ty == PTR || node->lhs->type->ty == ARRAY)
                node->type = node->lhs->type->ptr_to;
            else
                node->type = int_type;
            return;
        case ND_PTR_ADD:
        case ND_PTR_SUB:
            if (is_pointer(node->lhs))
                node->type = node->lhs->type;
            else
                node->type = node->rhs->type;
            return;
    }
}

Type *new_type(TypeKind ty, int size, Type *ptr_to, size_t array_size) {
    Type *type = calloc(1, sizeof(Type));
    type->ty = ty;
    type->size = size;
    type->ptr_to = ptr_to;
    type->array_size = array_size;
    return type;
}
