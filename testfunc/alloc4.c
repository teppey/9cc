#include <stdlib.h>

void alloc4(int **p, int a, int b, int c, int d) {
    *p = calloc(4, sizeof(int));
    *(*p + 0) = a;
    *(*p + 1) = b;
    *(*p + 2) = c;
    *(*p + 3) = d;
}
