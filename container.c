#include "sobacc.h"

int NODE_VECTOR_SIZE = 16;

NodeVector *new_node_vector() {
    NodeVector *vector = calloc(1, sizeof(NodeVector));
    vector->data = calloc(NODE_VECTOR_SIZE, sizeof(Node *));
    vector->size = NODE_VECTOR_SIZE;
    vector->count = 0;
    return vector;
}

// TODO: sizeを超える場合は領域を自動的に拡張する
void node_vector_add(NodeVector *vector, Node *node) {
    if (vector->count >= vector->size)
        error("node_vector_add: too many node");
    vector->data[vector->count++] = node;
}

Node *node_vector_ref(NodeVector *vector, int index) {
    if (index >= vector->count)
        error("node_vector_ref: index(%d) out of range", index);
    return vector->data[index];
}
