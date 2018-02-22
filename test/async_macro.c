#define ZPL_IMPLEMENTATION
#include <zpl.h>


typedef struct {
    i32 a,b,c;
} numbers;

ZPL_ASYNC_CB(do_calc) {
    numbers *n = cast(numbers *)data;
    n->a += n->c;
    n->b += n->a;
    n->c += n->b;
}

ZPL_ASYNC_CB(print_results) {
    numbers *n = cast(numbers *)data;

    zpl_printf("a: %d, b: %d, c: %d\n", n->a, n->b, n->c);
}

int main(void) {
    numbers t_ = {1, 2, 3};
    numbers *t = (numbers *)zpl_malloc(sizeof(numbers));
    *t = t_;
    zpl_async(t, do_calc, print_results);
    zpl_mfree(t);

    return 0;
}
