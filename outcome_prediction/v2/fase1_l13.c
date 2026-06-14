#include <stdio.h>

__attribute__((optimize("O0")))
int bench(long n_iter) {
    volatile int a = 0;
    long i = n_iter;
    do {
        if ((i % 13) == 0) a = 0;
    } while (--i > 0);
    return a;
}

int main() {
    return bench(10000000L);
}
